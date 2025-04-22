# QuickJS

ADD_LIBRARY(
    quickjs
    STATIC
    quickjs/quickjs.c
    quickjs/libregexp.c
    quickjs/libunicode.c
    quickjs/cutils.c
    quickjs/dtoa.c
)

TARGET_INCLUDE_DIRECTORIES(quickjs PUBLIC quickjs)
TARGET_LINK_LIBRARIES(quickjs PUBLIC tiny-libc)
TARGET_COMPILE_DEFINITIONS(quickjs PRIVATE "CONFIG_VERSION=\"2024-02-14\"")
TARGET_COMPILE_OPTIONS(quickjs PRIVATE -Wno-implicit-const-int-float-conversion)

# QuickJS-NG

ADD_LIBRARY(
    quickjs-ng
    STATIC
    quickjs-ng/quickjs.c
    quickjs-ng/libregexp.c
    quickjs-ng/libunicode.c
    quickjs-ng/cutils.c
    quickjs-ng/xsum.c
)

TARGET_INCLUDE_DIRECTORIES(quickjs-ng PUBLIC quickjs-ng)
TARGET_LINK_LIBRARIES(quickjs-ng PUBLIC tiny-libc)
TARGET_COMPILE_OPTIONS(quickjs-ng PRIVATE -Wno-implicit-const-int-float-conversion)