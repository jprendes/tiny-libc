fn main() {
    println!("cargo:rerun-if-changed=build.rs");

    let mut builder = cc::Build::new();
    builder.extra_warnings(false);

    builder.files(tiny_libc_build::sources());
    builder.includes(tiny_libc_build::includes());

    for flag in tiny_libc_build::cflags() {
        builder.flag(flag);
    }

    builder.compile("tiny-libc");
}
