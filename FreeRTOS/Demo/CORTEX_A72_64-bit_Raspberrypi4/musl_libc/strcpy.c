#include <stddef.h>  // for NULL

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    const char *s = src;

    // Copy each character until the null terminator
    while ((*d++ = *s++)) {
        // Nothing to do inside the loop
    }

    return dest;
}
