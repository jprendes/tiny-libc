use core::sync::atomic::Ordering;
use core::sync::atomic::{AtomicU32, AtomicUsize};
use core::time::Duration;
use core::{ptr, slice};

use linux_syscall::{syscall, Result as _, ResultPtr, ResultSize as _, SYS_mmap};
use linux_syscall::{SYS_exit, SYS_exit_group, SYS_futex, SYS_gettimeofday, SYS_read, SYS_write};

use crate::syscalls::{Errno, Syscalls};

pub struct LinuxSyscalls;

#[repr(align(65536))]
pub struct LinuxSyscallsPrealloc<const N: usize> {
    buf: [u8; N],
    heap: AtomicUsize,
}

impl LinuxSyscalls {
    pub const fn new() -> Self {
        Self
    }
}

impl<const N: usize> LinuxSyscallsPrealloc<N> {
    pub const fn new() -> Self {
        Self {
            buf: [0; N],
            heap: AtomicUsize::new(0),
        }
    }
}

impl From<linux_errno::Error> for Errno {
    fn from(err: linux_errno::Error) -> Self {
        Errno(err.get() as _)
    }
}

impl Syscalls for LinuxSyscalls {
    fn read(&self, fd: i32, mut buf: impl AsMut<[u8]>) -> Result<usize, Errno> {
        let buf = buf.as_mut();
        let res = unsafe { syscall!(SYS_read, fd, buf.as_mut_ptr(), buf.len()) };
        Ok(res.try_usize()?)
    }

    fn write(&self, fd: i32, buf: impl AsRef<[u8]>) -> Result<usize, Errno> {
        let buf = buf.as_ref();
        let res = unsafe { syscall!(SYS_write, fd, buf.as_ptr(), buf.len()) };
        Ok(res.try_usize()?)
    }

    fn exit(&self, ec: i32) -> ! {
        let _ = unsafe { syscall!(SYS_exit_group, ec) };
        loop {
            let _ = unsafe { syscall!(SYS_exit, ec) };
        }
    }

    fn wait(&self, addr: &AtomicU32, val: u32, timeout: Option<Duration>) -> Result<(), Errno> {
        const FUTEX_WAIT_PRIVATE: u32 = 0x80;
        let timeout = timeout.map(|t| [t.as_secs() as u64, t.subsec_nanos() as u64]);
        let timeout = timeout
            .as_ref()
            .map(|v| v as *const _)
            .unwrap_or(ptr::null());
        let res = unsafe { syscall!(SYS_futex, addr.as_ptr(), FUTEX_WAIT_PRIVATE, val, timeout) };
        Ok(res.check()?)
    }

    fn wake(&self, addr: &AtomicU32, nwake: u32) -> Result<u32, Errno> {
        const FUTEX_WAKE_PRIVATE: u32 = 0x81;
        let res = unsafe { syscall!(SYS_futex, addr.as_ptr(), FUTEX_WAKE_PRIVATE, nwake) };
        Ok(res.try_usize()? as _)
    }

    fn unixtime(&self) -> Result<Duration, Errno> {
        let ts = [0u64; 2];
        let res = unsafe { syscall!(SYS_gettimeofday, ts.as_ptr(), 0) };
        res.check()?;
        Ok(Duration::from_secs(ts[0]) + Duration::from_nanos(ts[1]))
    }

    fn page_alloc(&self, page_bits: usize, n_pages: usize) -> Result<&mut [u8], Errno> {
        const PROT_READ_WRITE: i32 = 0x1 | 0x2;
        const MAP_ANONYMOUS_PRIVATE: i32 = 0x20 | 0x02;
        let page_size = 1usize << page_bits;
        let size = page_size * n_pages + page_size;
        let res = unsafe {
            syscall!(
                SYS_mmap,
                0,
                size,
                PROT_READ_WRITE,
                MAP_ANONYMOUS_PRIVATE,
                -1,
                0
            )
        };
        let ptr = res.try_ptr_mut()? as *mut u8;
        let offset = match ptr.align_offset(page_size) {
            0 => page_size,
            offset => offset,
        };
        let ptr = unsafe { ptr.add(offset) };
        unsafe { (ptr as *mut *mut u8).sub(1).write(ptr) };
        Ok(unsafe { slice::from_raw_parts_mut(ptr, size) })
    }
}

impl<const N: usize> Syscalls for LinuxSyscallsPrealloc<N> {
    fn read(&self, fd: i32, buf: impl AsMut<[u8]>) -> Result<usize, Errno> {
        LinuxSyscalls.read(fd, buf)
    }

    fn write(&self, fd: i32, buf: impl AsRef<[u8]>) -> Result<usize, Errno> {
        LinuxSyscalls.write(fd, buf)
    }

    fn exit(&self, ec: i32) -> ! {
        LinuxSyscalls.exit(ec)
    }

    fn wait(&self, addr: &AtomicU32, val: u32, timeout: Option<Duration>) -> Result<(), Errno> {
        LinuxSyscalls.wait(addr, val, timeout)
    }

    fn wake(&self, addr: &AtomicU32, nwake: u32) -> Result<u32, Errno> {
        LinuxSyscalls.wake(addr, nwake)
    }

    fn unixtime(&self) -> Result<Duration, Errno> {
        LinuxSyscalls.unixtime()
    }

    fn page_alloc(&self, page_bits: usize, n_pages: usize) -> Result<&mut [u8], Errno> {
        let page_size = 1usize << page_bits;
        let offset = self.buf.as_ptr().align_offset(page_size);
        let _ = self
            .heap
            .compare_exchange(0, offset, Ordering::SeqCst, Ordering::SeqCst);
        let size = page_size * n_pages;
        let ptr = self.heap.fetch_add(size, Ordering::SeqCst);
        let ptr = unsafe { self.buf.as_ptr().add(ptr) as *mut _ };
        Ok(unsafe { slice::from_raw_parts_mut(ptr, size) })
    }
}
