# tiny-libc

A libc implementation large enough to compile [QuickJS](https://github.com/bellard/quickjs)([-NG](https://github.com/quickjs-ng/quickjs)), and probably nothing else.

## Building

You can build with either `make` or `cmake`.

### Building with `make`
```
make CC=clang all
./build/demo
./build/demo-ng
```

### Build with `cmake`
```
cmake --fresh -B ./build/ -S ./ -DCMAKE_C_COMPILER=clang
cmake --build ./build/ -- demo demo-ng
./build/demo
./build/demo-ng
```

## Standing on the shoulders of giants
The libc implementation in this repo borrows from many sources:
* libm: [musl](https://github.com/kraj/musl/) [MIT]
* crt: [llvm](https://github.com/llvm-mirror/compiler-rt/) [Apache-2.0 with LLVM Exceptions]
* malloc: [walloc](https://github.com/wingo/walloc) [MIT]
* mutex: [tchajed](https://github.com/tchajed/futex-tutorial) [MIT]
* condvar: [rust](https://github.com/rust-lang/rust/blob/master/library/std/src/sys/sync/condvar/futex.rs) [MIT OR Apache-2.0]
* printf: [mpaland](https://github.com/mpaland/printf) [MIT]
