#include <string.h>

size_t strlen(const char *str) {
    const char *s;
    for (s = str; *s; ++s) ;
    return s - str;
}

void *memcpy(void *dest, const void *src, size_t len) {
    char *d = dest;
    const char *s = src;
    while (len--) *d++ = *s++;
    return dest;
}

void *memmove(void *dest, const void *src, size_t len) {
    char *d = dest;
    const char *s = src;

    if (s < d && s + len > d) {
        s += len;
        d += len;
        while (len-- > 0) *--d = *--s;
    } else {
        while (len-- > 0) *d++ = *s++;
    }

    return dest;
}

void *memset(void *dest, int c, size_t len) {
    char v = c;
    char * volatile d = dest;
    while (len--) *d++ = v;
    return dest;
}

int memcmp(const void *v1, const void *v2, size_t len) {
    const char *s1 = v1;
    const char *s2 = v2;

    while (len--) {
        if (*s1 < *s2) return -1;
        if (*s1++ > *s2++) return 1;
    }
    return 0;
}

int bcmp(const void *v1, const void *v2, size_t len) {
	return memcmp(v1, v2, len);
}

char *strchr(const char *s, int c) {
    while (*s != c) {
        if (!*s++) return NULL;
    }
    return (char *)s;
}

char *strrchr(const char *s, int c) {
    size_t len = strlen(s);
    const char *sr = s + len;
    while (*--sr != c) {
        if (sr == s) return NULL;
    }
    return (char *)sr;
}

int atoi(const char *p) {
    int k = 0;
    int negate = 0;
    if (*p == '-' || *p == '+') {
        negate = *p++ == '-';
    }
    while (*p) {
        if (*p < '0' || *p > '9') break;
        k = 10 * k + (*p) - '0';
        p++;
    }
    return negate ? -k : k;
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while((*dest++=*src++) != '\0');
    return d;
}

int strcmp(const char* s1, const char* s2) {
    while(*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char *strcat(char *dest, const char *src) {
    char *rdest = dest;
    while (*dest) dest++;
    while ((*dest++ = *src++)) ;
    return rdest;
}

void *memchr(const void *ptr, int c, size_t len ) {
    const char *s = ptr;
    while (len--) {
        if (*s == c) return (char *)s;
    }
    return NULL;
}

char* strstr(const char* str, const char* substr) {
    size_t len = strlen(substr);
    while (*str) {
        if (strncmp(str, substr, len) == 0) return (char *)str;
        str++;
    }
    return NULL;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    while(n && *s1 && *s1 == *s2) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}
