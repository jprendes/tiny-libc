use core::ffi::c_void;
use core::fmt::Write as _;

use super::Errno;

extern "C" {
    pub(super) fn __stdin() -> *const c_void;
    pub(super) fn __stdout() -> *const c_void;
    pub(super) fn __stderr() -> *const c_void;
    pub(super) fn fflush(fp: *const c_void) -> i32;
    pub(super) fn fwrite(buf: *const c_void, size: usize, count: usize, fp: *const c_void)
        -> usize;
    pub(super) fn fread(buf: *mut c_void, size: usize, count: usize, fp: *const c_void) -> usize;
}

const EIO: i32 = 5;

fn write(fp: *const c_void, buf: impl AsRef<[u8]>) -> Result<usize, Errno> {
    let buf = buf.as_ref();
    match unsafe { fwrite(buf.as_ptr() as _, 1, buf.len(), fp) } {
        n if n <= buf.len() => Ok(n),
        _ => Err(Errno(EIO)),
    }
}

fn read(fp: *const c_void, mut buf: impl AsMut<[u8]>) -> Result<usize, Errno> {
    let buf = buf.as_mut();
    match unsafe { fread(buf.as_mut_ptr() as _, 1, buf.len(), fp) } {
        n if n <= buf.len() => Ok(n),
        _ => Err(Errno(EIO)),
    }
}

fn flush(fp: *const c_void) -> Result<(), Errno> {
    match unsafe { fflush(fp) } {
        0 => Ok(()),
        _ => Err(Errno(EIO)),
    }
}

pub trait Read {
    fn read(&self, buf: &mut [u8]) -> Result<usize, Errno>;
}

pub trait Write {
    fn write(&mut self, buf: &[u8]) -> Result<usize, Errno>;
    fn flush(&mut self) -> Result<(), Errno>;
}

pub struct Stdin;
pub struct Stdout;
pub struct Stderr;

pub fn stdin() -> Stdin {
    Stdin
}

pub fn stdout() -> Stdout {
    Stdout
}

pub fn stderr() -> Stderr {
    Stderr
}

impl Read for Stdin {
    fn read(&self, buf: &mut [u8]) -> Result<usize, Errno> {
        read(unsafe { __stdin() }, buf)
    }
}

impl Stdin {
    #[cfg(feature = "alloc")]
    pub fn read_line(&self, buf: &mut alloc::string::String) -> Result<usize, Errno> {
        let mut bytes = alloc::vec![];
        loop {
            let mut buf = [0u8; 1024];
            // our implementation of read stops at newline
            let n = self.read(&mut buf)?;
            bytes.extend(&buf[..n]);
            if n == 0 || buf[n - 1] == b'\n' {
                break;
            }
        }
        let bytes = core::str::from_utf8(&bytes).map_err(|_| Errno(EIO))?;
        buf.push_str(bytes);
        Ok(bytes.len())
    }
}

impl Write for Stdout {
    fn write(&mut self, buf: &[u8]) -> Result<usize, Errno> {
        write(unsafe { __stdout() }, buf)
    }

    fn flush(&mut self) -> Result<(), Errno> {
        flush(unsafe { __stdout() })
    }
}

impl core::fmt::Write for Stdout {
    fn write_str(&mut self, s: &str) -> core::fmt::Result {
        self.write(s.as_bytes()).map_err(|_| core::fmt::Error)?;
        Ok(())
    }
}

impl Write for Stderr {
    fn write(&mut self, buf: &[u8]) -> Result<usize, Errno> {
        write(unsafe { __stderr() }, buf)
    }

    fn flush(&mut self) -> Result<(), Errno> {
        flush(unsafe { __stderr() })
    }
}

impl core::fmt::Write for Stderr {
    fn write_str(&mut self, s: &str) -> core::fmt::Result {
        self.write(s.as_bytes()).map_err(|_| core::fmt::Error)?;
        Ok(())
    }
}

#[doc(hidden)]
pub fn _print(args: core::fmt::Arguments) {
    let _ = Stdout.write_fmt(args);
}

#[doc(hidden)]
pub fn _eprint(args: core::fmt::Arguments) {
    let _ = Stderr.write_fmt(args);
}

#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => {
        $crate::io::_print(core::format_args!($($arg)*));
    };
}

#[macro_export]
macro_rules! println {
    () => {
        $crate::print!("\n");
    };
    ($($arg:tt)*) => {{
        $crate::print!($($arg)*);
        $crate::print!("\n");
    }};
}

#[macro_export]
macro_rules! eprint {
    ($($arg:tt)*) => {
        $crate::io::_eprint(core::format_args!($($arg)*));
    };
}

#[macro_export]
macro_rules! eprintln {
    () => {
        $crate::eprint!("\n");
    };
    ($($arg:tt)*) => {{
        $crate::eprint!($($arg)*);
        $crate::eprint!("\n");
    }};
}
