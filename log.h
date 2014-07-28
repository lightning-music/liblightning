#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED

#include <stdarg.h>
#include <stdio.h>

void
lightning_log(const char *file,
              long line,
              int level,
              const char *fmt,
              ...);

#define LOG(level, fmt, args...) lightning_log(__FILE__, __LINE__, level, fmt, args)

#endif
