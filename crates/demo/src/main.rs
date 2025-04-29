#![no_std]
#![no_main]

extern crate alloc;

use alloc::string::String;

use rquickjs_core::{CatchResultExt, Context, Function, Object, Result, Runtime, Value};

use tiny_libc::io::Write as _;
use tiny_libc::{use_syscalls, LinuxSyscalls, LibcAllocator};
use tiny_libc::{print, println, eprintln};
use tiny_libc::{io, process};

#[use_syscalls]
static SYSCALLS: LinuxSyscalls = LinuxSyscalls::new();

#[global_allocator]
pub(crate) static ALLOCATOR: LibcAllocator = LibcAllocator;

#[cfg(not(test))]
#[panic_handler]
fn panic(_: &core::panic::PanicInfo) -> ! {
    let _ = eprintln!("panic occurred, aborting...");
    process::abort();
}

fn print(s: String) {
    println!("{s}");
}

#[no_mangle]
fn main() -> core::ffi::c_int {
    return if main_impl().is_ok() { 0 } else { 1 };
}

fn main_impl() -> Result<()> {
    let rt = Runtime::new()?;
    let ctx = Context::full(&rt)?;

    ctx.with(|ctx| -> Result<()> {
        let global = ctx.globals();
        global.set(
            "__print",
            Function::new(ctx.clone(), print)?.with_name("__print")?,
        )?;
        ctx.eval::<(), _>(
            r#"
globalThis.console = {
  log(...v) {
    globalThis.__print(`${v.join(" ")}`)
  }
}
"#,
        )?;

        let console: Object = global.get("console")?;
        let js_log: Function = console.get("log")?;
        loop {
            let mut input = String::new();
            print!("> ");
            io::stdout().flush().unwrap();
            io::stdin().read_line(&mut input).unwrap();
            if input.is_empty() {
                println!();
                break Ok(());
            };
            ctx.eval::<Value, _>(input.as_bytes())
                .and_then(|ret| js_log.call::<(Value<'_>,), ()>((ret,)))
                .catch(&ctx)
                .unwrap_or_else(|err| println!("{err}"));
        }
    })?;

    Ok(())
}
