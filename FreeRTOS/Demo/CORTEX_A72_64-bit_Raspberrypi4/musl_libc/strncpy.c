#include <stddef.h>  // for size_t

char *strncpy(char *dest, const char *src, size_t n) {
    char *d = dest;
    const char *s = src;

    // Copy up to n characters from src to dest
    while (n--) {
        if ((*d++ = *s++) == '\0') {
            // If we reach the null terminator, pad the rest with '\0'
            while (n--) {
                *d++ = '\0';
            }
            break;
        }
    }

    return dest;
}
