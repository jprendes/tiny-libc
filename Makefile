CC ?= clang
OPT_FLAGS := -Os -flto=auto

.PHONY: all build build-ng

all: build build-ng

build:
	@mkdir -p ./build
	@echo Compiling quickjs demo
	@"${CC}" \
	    -I./upstream/quickjs/ \
	    ./upstream/quickjs/quickjs.c \
	    ./upstream/quickjs/libregexp.c \
	    ./upstream/quickjs/libunicode.c \
	    ./upstream/quickjs/cutils.c \
	    ./upstream/quickjs/dtoa.c \
	    ./main.c \
	    -I./libc/include \
	    ./libc/arch/x86_64-linux.c \
	    ./libc/src/*.c \
	    ./libc/src/math/*.c \
	    ./libc/src/pthread/*.c \
	    -DCONFIG_VERSION=\"2024-02-14\" \
	    -nostdinc \
	    -nostdlib \
	    ${OPT_FLAGS} \
	    -fno-stack-protector \
	    -Wno-shift-op-parentheses \
	    -Wno-implicit-const-int-float-conversion \
	    -o ./build/demo

build-ng:
	@mkdir -p ./build
	@echo Compiling quickjs-ng demo
	@"${CC}" \
	    -I./upstream/quickjs-ng/ \
	    ./upstream/quickjs-ng/quickjs.c \
	    ./upstream/quickjs-ng/libregexp.c \
	    ./upstream/quickjs-ng/libunicode.c \
	    ./upstream/quickjs-ng/cutils.c \
	    ./upstream/quickjs-ng/xsum.c \
	    ./main.c \
	    -I./libc/include \
	    ./libc/arch/x86_64-linux.c \
	    ./libc/src/*.c \
	    ./libc/src/math/*.c \
	    ./libc/src/pthread/*.c \
	    -nostdinc \
	    -nostdlib \
	    ${OPT_FLAGS} \
	    -fno-stack-protector \
	    -Wno-shift-op-parentheses \
	    -Wno-implicit-const-int-float-conversion \
	    -o ./build/demo-ng
