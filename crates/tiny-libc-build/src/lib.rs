use std::fs::read_dir;
use std::path::{Path, PathBuf};

fn traverse_dir(src: impl AsRef<Path>, fcn: &mut impl FnMut(&Path)) {
    let src = src.as_ref();

    for entry in read_dir(src).unwrap() {
        let entry = entry.unwrap();
        let path = entry.path();

        if path.is_dir() {
            traverse_dir(path, fcn);
        } else {
            fcn(&path, );
        }
    }
}

pub fn sources() -> Vec<PathBuf> {
    let mut files = vec![];
    traverse_dir(concat!(env!("OUT_DIR"), "/src"), &mut |path| {
        if path.extension() == Some("c".as_ref()) {
            files.push(path.to_path_buf());
        }
    });
    return files;
}

pub fn includes() -> Vec<PathBuf> {
    vec![PathBuf::from(concat!(env!("OUT_DIR"), "/include"))]
}

pub fn cflags() -> Vec<String> {
    vec![
        "-nostdlib".into(),
        "-nostdinc".into(),
        "-mstack-protector-guard=global".into(),
    ]
}