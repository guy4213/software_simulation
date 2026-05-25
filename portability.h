#ifndef PORTABILITY_H
#define PORTABILITY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // time values for sleep on Linux

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h> // Windows API, including Sleep
#else
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <stdarg.h> // variable-argument helper functions
#include <errno.h>  // standard error codes
#include <unistd.h> // Unix/POSIX functions

static inline int fopen_s(FILE **pFile, const char *name, const char *mode)
{
    if (!pFile)
        return EINVAL;
    *pFile = fopen(name, mode);
    return (*pFile ? 0 : errno);
}

static inline int sprintf_s(char *buf, size_t n, const char *fmt, ...)
{
    va_list a;
    va_start(a, fmt);
    int r = vsnprintf(buf, n, fmt, a);
    va_end(a);
    return (r < 0 || r >= (int)n) ? -1 : 0;
}

static inline int strcpy_s(char *d, size_t n, const char *s)
{
    if (strlen(s) + 1 > n)
    {
        if (n)
            d[0] = 0;
        return ERANGE;
    }
    strncpy(d, s, n);
    d[n - 1] = 0;
    return 0;
}

static inline int sscanf_s(const char *b, const char *f, ...)
{
    va_list a;
    va_start(a, f);
    int r = vsscanf(b, f, a);
    va_end(a);
    return r;
}

static inline int scanf_s(const char *f, ...)
{
    va_list a;
    va_start(a, f);
    int r = vscanf(f, a);
    va_end(a);
    return r;
}

extern int nanosleep(const struct timespec *req, struct timespec *rem);
#define Sleep(ms)                                                         \
    do                                                                    \
    {                                                                     \
        struct timespec _ts = {(ms) / 1000, ((ms) % 1000) * 1000000};     \
        nanosleep(&_ts, NULL); /* pause for the requested milliseconds */ \
    } while (0)
#endif

#endif