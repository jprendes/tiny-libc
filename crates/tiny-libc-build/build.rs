use std::env::var;
use std::fs::{copy, create_dir_all};
use std::path::{Path, PathBuf};

fn copy_dir_recursive(src: impl AsRef<Path>, dst: impl AsRef<Path>) {
    let src = src.as_ref();
    let dst = dst.as_ref();

    if !dst.exists() {
        create_dir_all(dst).unwrap();
    }

    for entry in std::fs::read_dir(src).unwrap() {
        let entry = entry.unwrap();
        let path = entry.path();
        let relative_path = path.strip_prefix(src).unwrap();
        let new_path = dst.join(relative_path);

        if path.is_dir() {
            copy_dir_recursive(path, new_path);
        } else {
            println!("cargo:rerun-if-changed={}", path.display());
            copy(path, new_path).unwrap();
        }
    }
}

fn main() {
    println!("cargo:rerun-if-changed=build.rs");

    let out_dir = var("OUT_DIR").unwrap();
    let out_dir = PathBuf::from(out_dir);

    let libc = Path::new("../../libc");

    copy_dir_recursive(libc.join("src"), out_dir.join("src"));
    copy_dir_recursive(libc.join("include"), out_dir.join("include"));

    println!("cargo:rerun-if-changed=/home/jprendes/Projects/quickjs-standalone/libc/arch/x86_64-linux.c");
}
