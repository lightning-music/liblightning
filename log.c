#include <stdarg.h>

#include "log.h"

void
lightning_log(const char *file,
              long line,
              int level,
              const char *fmt,
              ...) {
    va_list ap;
    char msg[128];
    va_start(ap, fmt);
    vsprintf(msg, fmt, ap);
    printf("[%s:%ld] %s\n", file, line, msg);
}
