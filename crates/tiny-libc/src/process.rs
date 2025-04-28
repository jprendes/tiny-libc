mod bindings {
    use core::ffi::c_int;

    extern "C" {
        pub(super) fn abort() -> !;
        pub(super) fn exit(ec: c_int) -> !;
    }
}

pub fn abort() -> ! {
    unsafe { bindings::abort() }
}

pub fn exit(code: i32) -> ! {
    unsafe { bindings::exit(code as _) }
}