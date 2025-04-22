FILE(GLOB_RECURSE TINY_LIBC_SRC_FILES src/*.c)

ADD_LIBRARY(
    tiny-libc
    OBJECT
    ${TINY_LIBC_SRC_FILES}
    arch/x86_64-linux.c
)

TARGET_INCLUDE_DIRECTORIES(tiny-libc PUBLIC include)
TARGET_COMPILE_OPTIONS(tiny-libc PUBLIC -nostdinc -fno-stack-protector)
TARGET_LINK_OPTIONS(tiny-libc INTERFACE -nostdlib)
