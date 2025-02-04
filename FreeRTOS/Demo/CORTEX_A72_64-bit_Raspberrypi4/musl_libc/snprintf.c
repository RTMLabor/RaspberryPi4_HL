#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

int vsnprintf(char *str, size_t size, const char *format, va_list ap) {
    int ret;

    if (size == 0) {
        // Null buffer, calculate length only
        char temp[1];
        ret = vsnprintf(temp, sizeof(temp), format, ap);
    } else {
        // Write formatted data to the provided buffer
        ret = vsnprintf(str, size, format, ap);
        if (size > 0) {
            str[size - 1] = '\0'; // Null-terminate the string
        }
    }

    return ret;
}



int snprintf(char *restrict s, size_t n, const char *restrict fmt, ...)
{
    va_list ap;
    int ret;

    // Initialize the variable argument list
    va_start(ap, fmt);

    // Call vsnprintf, which performs the actual formatted writing
    ret = vsnprintf(s, n, fmt, ap);

    // Clean up the variable argument list
    va_end(ap);

    return ret;
}
