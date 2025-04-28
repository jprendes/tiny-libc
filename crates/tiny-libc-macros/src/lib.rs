use proc_macro::TokenStream;
use proc_macro2::Span;
use proc_macro_crate::{crate_name, FoundCrate};
use quote::quote;
use syn::{parse_macro_input, Ident, ItemStatic};

#[proc_macro_attribute]
pub fn use_syscalls(_attr: TokenStream, item: TokenStream) -> TokenStream {
    let static_declaration = parse_macro_input!(item as ItemStatic);
    let ident = static_declaration.ident.clone();
    let ident = quote! { super::#ident };
    let crate_name = crate_name("tiny-libc").expect("tiny-libc must be a dependency");
    let crate_name = match crate_name {
        FoundCrate::Itself => quote! {::crate},
        FoundCrate::Name(name) => {
            let ident: Ident = Ident::new(&name, Span::call_site());
            quote! {::#ident}
        }
    };

    quote! {
        #static_declaration

        mod __tiny_libc_syscalls {
            #[no_mangle]
            extern "C" fn __read(
                fd: ::core::ffi::c_int,
                buffer: *mut ::core::ffi::c_void,
                len: usize,
            ) -> ::core::ffi::c_int {
                let buffer = unsafe { ::core::slice::from_raw_parts_mut(buffer as _, len) };
                match #crate_name::Syscalls::read(&#ident, fd, buffer) {
                    Ok(n) => n as _,
                    Err(err) => -err.0,
                }
            }

            #[no_mangle]
            extern "C" fn __write(
                fd: ::core::ffi::c_int,
                buffer: *const ::core::ffi::c_void,
                len: usize,
            ) -> ::core::ffi::c_int {
                let buffer = unsafe { ::core::slice::from_raw_parts(buffer as _, len) };
                match #crate_name::Syscalls::write(&#ident, fd, buffer) {
                    Ok(n) => n as _,
                    Err(err) => -err.0,
                }
            }

            #[no_mangle]
            extern "C" fn __exit(ec: ::core::ffi::c_int) -> ! {
                #crate_name::Syscalls::exit(&#ident, ec);
            }

            #[no_mangle]
            extern "C" fn __wait(addr: *const u32, val: u32, timeout: u64) -> ::core::ffi::c_int {
                let addr = unsafe { &*(addr as *const ::core::sync::atomic::AtomicU32) };
                let timeout = if timeout > 0 {
                    Some(::core::time::Duration::from_micros(timeout * 1000))
                } else {
                    None
                };
                match #crate_name::Syscalls::wait(&#ident, addr, val, timeout) {
                    Ok(()) => 0,
                    Err(err) => -err.0,
                }
            }

            #[no_mangle]
            extern "C" fn __wake(addr: *const u32, nwake: u32) -> ::core::ffi::c_int {
                let addr = unsafe { &*(addr as *const ::core::sync::atomic::AtomicU32) };
                match #crate_name::Syscalls::wake(&#ident, addr, nwake) {
                    Ok(n) => n as _,
                    Err(err) => -err.0,
                }
            }

            #[no_mangle]
            extern "C" fn __unixtime_nsec() -> u128 {
                #crate_name::Syscalls::unixtime(&#ident)
                    .unwrap_or_default()
                    .as_nanos()
            }

            #[no_mangle]
            extern "C" fn __page_alloc(page_bits: usize, n_pages: usize) -> *mut ::core::ffi::c_void {
                match #crate_name::Syscalls::page_alloc(&#ident, page_bits, n_pages) {
                    Ok(mem) => mem.as_mut_ptr() as _,
                    Err(err) => ::core::ptr::null_mut(),
                }
            }
        }
    }
    .into()
}
