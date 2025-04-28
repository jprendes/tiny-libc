#pragma once

#include <malloc.h>
#include <stddef.h>

int atexit(void (*func)(void));
void exit(int status);
void abort(void);

double strtod(const char * string, char ** endPtr);
static inline int abs(int a) {
	return a>0 ? a : -a;
}
