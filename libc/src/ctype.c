#include <ctype.h>

int isdigit(int c) {
   return c >= '0' && c <= '9';
}

int isalpha(int c) {
   return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isspace(int c) {
   return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}
