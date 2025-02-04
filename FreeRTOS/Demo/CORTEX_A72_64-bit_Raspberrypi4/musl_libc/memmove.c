#include <stddef.h>  // for size_t

void *memmove(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;

    // Check if the memory regions overlap
    if (s < d && d < s + n) {
        // If the destination is before the source, copy backwards
        s += n;
        d += n;
        while (n--) {
            *(--d) = *(--s);
        }
    } else {
        // Otherwise, copy forwards
        while (n--) {
            *d++ = *s++;
        }
    }

    return dest;
}
