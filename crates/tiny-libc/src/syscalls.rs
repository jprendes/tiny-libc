use core::sync::atomic::AtomicU32;
use core::time::Duration;

#[derive(Copy, Clone, Eq, Ord, PartialEq, PartialOrd, Hash, Debug)]
pub struct Errno(pub i32);

pub trait Syscalls: Send + Sync {
    fn read(&self, fd: i32, buf: impl AsMut<[u8]>) -> Result<usize, Errno>;
    fn write(&self, fd: i32, buf: impl AsRef<[u8]>) -> Result<usize, Errno>;
    fn exit(&self, ec: i32) -> !;
    fn wait(&self, addr: &AtomicU32, val: u32, timeout: Option<Duration>) -> Result<(), Errno>;
    fn wake(&self, addr: &AtomicU32, nwake: u32) -> Result<u32, Errno>;
    fn unixtime(&self) -> Result<core::time::Duration, Errno>;
    fn page_alloc(&self, page_bits: usize, n_pages: usize) -> Result<&mut [u8], Errno>;
}
