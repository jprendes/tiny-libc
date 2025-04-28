#![no_std]
#![no_main]

use tiny_libc::{use_syscalls, LibcAllocator, LinuxSyscalls};
use tiny_libc::{eprintln, println};
use tiny_libc::process::abort;

#[use_syscalls]
static SYSCALLS: LinuxSyscalls = LinuxSyscalls;

#[global_allocator]
static ALLOCATOR: LibcAllocator = LibcAllocator;

#[cfg(not(test))]
#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    eprintln!("panic!");
    abort();
}

#[no_mangle]
extern "C" fn main() -> core::ffi::c_int {
    println!("hello world!");
    0
}