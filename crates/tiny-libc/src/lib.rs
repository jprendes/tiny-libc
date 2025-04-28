#![no_std]

#[cfg(feature = "alloc")]
extern crate alloc;

#[cfg(feature = "linux")]
mod linux;
mod syscalls;
mod allocator;
pub mod io;
pub mod process;

#[cfg(feature = "linux")]
pub use linux::{LinuxSyscalls, LinuxSyscallsPrealloc};
pub use syscalls::{Errno, Syscalls};
pub use allocator::LibcAllocator;

pub use tiny_libc_macros::use_syscalls;
