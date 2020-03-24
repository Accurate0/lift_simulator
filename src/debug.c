#include <stdio.h>
#include <stdarg.h>

#include "debug.h"

int d_printf(const char *s, ...)
{
#ifdef DEBUG
    va_list list;
    va_start(list, s);
    int n = vprintf(s, list);
    va_end(list);
    return n;
#else
    (void)s;
    return 0;
#endif
}
