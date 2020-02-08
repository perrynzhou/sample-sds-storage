
/*************************************************************************
  > File Name: log.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Sun 24 Nov 2019 10:52:54 AM CST
 ************************************************************************/
#include "log.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
static int _scnprintf(char *buf, size_t size, const char *fmt, ...);
static int _safe_vsnprintf(char *to, size_t size, const char *format, va_list ap);
static int _safe_snprintf(char *to, size_t n, const char *fmt, ...);
static int _vscnprintf(char *buf, size_t size, const char *fmt, va_list args);
static int _safe_vsnprintf(char *to, size_t size, const char *format, va_list ap);
static int _safe_vsnprintf(char *to, size_t size, const char *format, va_list ap);
static void _log_stderr(const char *fmt, ...);
static void _log_stderr_safe(const char *fmt, ...);
#define _log_snprintf(_s, _n, ...) \
    snprintf((char *)(_s), (size_t)(_n), __VA_ARGS__)

#define _log_scnprintf(_s, _n, ...) \
    _scnprintf((char *)(_s), (size_t)(_n), __VA_ARGS__)

#define _log_vsnprintf(_s, _n, _f, _a) \
    vsnprintf((char *)(_s), (size_t)(_n), _f, _a)

#define _log_vscnprintf(_s, _n, _f, _a) \
    _vscnprintf((char *)(_s), (size_t)(_n), _f, _a)

#define _log_strftime(_s, _n, fmt, tm) \
    (int)strftime((char *)(_s), (size_t)(_n), fmt, tm)
#define _log_write(_d, _b, _n) \
    write(_d, _b, (size_t)(_n))
#define _log_safe_snprintf(_s, _n, ...) \
    _safe_snprintf((char *)(_s), (size_t)(_n), __VA_ARGS__)

#define _log_safe_vsnprintf(_s, _n, _f, _a) \
    _safe_vsnprintf((char *)(_s), (size_t)(_n), _f, _a)

#define log_stderr(...)           \
    do                            \
    {                             \
        _log_stderr(__VA_ARGS__); \
    } while (0)

#define log_stderr_safe(...)           \
    do                                 \
    {                                  \
        _log_stderr_safe(__VA_ARGS__); \
    } while (0)
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
static struct logger logger;
const char *logger_level_names[] = {
    "ERR",
    "WARN",
    "INFO",
    "DEBUG",
    "TRACE",
};
static const char *logger_level_colors[] = {
    "\x1b[35m",
    "\x1b[36m",
    "\x1b[32m",
    "\x1b[33m",
    "\x1b[31m"};
int log_init(int level, int output_type, char *filename)
{
    struct logger *l = &logger;
    l->level = MAX(LOG_ERR, MIN(level, LOG_TRACE));
    l->name = filename;
    if (output_type < LOG_STDOUT_TYPE || output_type > LOG_DEFINE_TYPE)
    {
        return -1;
    }
    if (output_type == LOG_STDOUT_TYPE)
    {
        l->fd = STDOUT_FILENO;
    }
    else if (output_type == LOG_STDERR_TYPE)
    {
        l->fd = STDERR_FILENO;
    }
    else
    {
        if (filename == NULL || !strlen(filename))
        {
            l->fd = STDOUT_FILENO;
        }
        else
        {
            l->fd = open(filename, O_WRONLY | O_APPEND | O_CREAT, 0644);
            if (l->fd < 0)
            {
                log_stderr("opening log file '%s' failed: %s", filename,
                           strerror(errno));
                return -1;
            }
        }
    }
    log_safe("init log success,output to %d", l->fd);
    return 0;
}
void _log_safe(const char *file, int line, int panic, const char *fmt, ...)
{
    struct logger *l = &logger;
    int len, size, errno_save;
    char buf[LOG_MAX_LEN];
    va_list args;
    ssize_t n;

    if (l->fd < 0)
    {
        return;
    }
    struct timeval tv;
    errno_save = errno;
    len = 0;            /* length of output buffer */
    size = LOG_MAX_LEN; /* size of output buffer */
    gettimeofday(&tv, NULL);
    buf[len++] = '[';
    len += _log_strftime(buf + len, size - len, "%Y-%m-%d %H:%M:%S.", localtime(&tv.tv_sec));
    len += _log_safe_snprintf(buf + len, size - len, "%03ld", tv.tv_usec / 1000);
    len += _log_safe_snprintf(buf + len, size - len, "] [%s%s\x1b[0m \x1b[90m] %s:%d ", logger_level_colors[l->level], logger_level_names[l->level], file, line);

    va_start(args, fmt);
    len += _log_safe_vsnprintf(buf + len, size - len, fmt, args);
    va_end(args);

    buf[len++] = '\n';

    n = _log_write(l->fd, buf, len);
    if (n < 0)
    {
        l->nerror++;
    }

    errno = errno_save;
}

static int
_vscnprintf(char *buf, size_t size, const char *fmt, va_list args)
{
    int n;

    n = vsnprintf(buf, size, fmt, args);
    if (n <= 0)
    {
        return 0;
    }

    if (n < (int)size)
    {
        return n;
    }

    return (int)(size - 1);
}
static void
_log_stderr(const char *fmt, ...)
{
    struct logger *l = &logger;
    int len, size, errno_save;
    char buf[4 * LOG_MAX_LEN];
    va_list args;
    ssize_t n;

    errno_save = errno;
    len = 0;                /* length of output buffer */
    size = 4 * LOG_MAX_LEN; /* size of output buffer */

    va_start(args, fmt);
    len += _vscnprintf(buf, size, fmt, args);
    va_end(args);

    buf[len++] = '\n';

    n = _log_write(STDERR_FILENO, buf, len);
    if (n < 0)
    {
        l->nerror++;
    }

    errno = errno_save;
}

static int
_scnprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list args;
    int n;

    va_start(args, fmt);
    n = _vscnprintf(buf, size, fmt, args);
    va_end(args);

    return n;
}
static char *
_safe_utoa(int _base, uint64_t val, char *buf)
{
    char hex[] = "0123456789abcdef";
    uint32_t base = (uint32_t)_base;
    *buf-- = 0;
    do
    {
        *buf-- = hex[val % base];
    } while ((val /= base) != 0);
    return buf + 1;
}

static char *
_safe_itoa(int base, int64_t val, char *buf)
{
    char hex[] = "0123456789abcdef";
    char *orig_buf = buf;
    const int32_t is_neg = (val < 0);
    *buf-- = 0;

    if (is_neg)
    {
        val = -val;
    }
    if (is_neg && base == 16)
    {
        int ix;
        val -= 1;
        for (ix = 0; ix < 16; ++ix)
            buf[-ix] = '0';
    }

    do
    {
        *buf-- = hex[val % base];
    } while ((val /= base) != 0);

    if (is_neg && base == 10)
    {
        *buf-- = '-';
    }

    if (is_neg && base == 16)
    {
        int ix;
        buf = orig_buf - 1;
        for (ix = 0; ix < 16; ++ix, --buf)
        {
            /* *INDENT-OFF* */
            switch (*buf)
            {
            case '0':
                *buf = 'f';
                break;
            case '1':
                *buf = 'e';
                break;
            case '2':
                *buf = 'd';
                break;
            case '3':
                *buf = 'c';
                break;
            case '4':
                *buf = 'b';
                break;
            case '5':
                *buf = 'a';
                break;
            case '6':
                *buf = '9';
                break;
            case '7':
                *buf = '8';
                break;
            case '8':
                *buf = '7';
                break;
            case '9':
                *buf = '6';
                break;
            case 'a':
                *buf = '5';
                break;
            case 'b':
                *buf = '4';
                break;
            case 'c':
                *buf = '3';
                break;
            case 'd':
                *buf = '2';
                break;
            case 'e':
                *buf = '1';
                break;
            case 'f':
                *buf = '0';
                break;
            }
            /* *INDENT-ON* */
        }
    }
    return buf + 1;
}

static const char *
_safe_check_longlong(const char *fmt, int32_t *have_longlong)
{
    *have_longlong = false;
    if (*fmt == 'l')
    {
        fmt++;
        if (*fmt != 'l')
        {
            *have_longlong = (sizeof(long) == sizeof(int64_t));
        }
        else
        {
            fmt++;
            *have_longlong = true;
        }
    }
    return fmt;
}
static void _log_stderr_safe(const char *fmt, ...)
{
    struct logger *l = &logger;
    int len, size, errno_save;
    char buf[LOG_MAX_LEN];
    va_list args;
    ssize_t n;

    errno_save = errno;
    len = 0;            /* length of output buffer */
    size = LOG_MAX_LEN; /* size of output buffer */

    len += _log_safe_snprintf(buf + len, size - len, "[.......................] ");

    va_start(args, fmt);
    len += _log_safe_vsnprintf(buf + len, size - len, fmt, args);
    va_end(args);

    buf[len++] = '\n';

    n = _log_write(STDERR_FILENO, buf, len);
    if (n < 0)
    {
        l->nerror++;
    }

    errno = errno_save;
}

static int _safe_vsnprintf(char *to, size_t size, const char *format, va_list ap)
{
    char *start = to;
    char *end = start + size - 1;
    for (; *format; ++format)
    {
        int32_t have_longlong = false;
        if (*format != '%')
        {
            if (to == end)
            { /* end of buffer */
                break;
            }
            *to++ = *format; /* copy ordinary char */
            continue;
        }
        ++format; /* skip '%' */

        format = _safe_check_longlong(format, &have_longlong);

        switch (*format)
        {
        case 'd':
        case 'i':
        case 'u':
        case 'x':
        case 'p':
        {
            int64_t ival = 0;
            uint64_t uval = 0;
            if (*format == 'p')
                have_longlong = (sizeof(void *) == sizeof(uint64_t));
            if (have_longlong)
            {
                if (*format == 'u')
                {
                    uval = va_arg(ap, uint64_t);
                }
                else
                {
                    ival = va_arg(ap, int64_t);
                }
            }
            else
            {
                if (*format == 'u')
                {
                    uval = va_arg(ap, uint32_t);
                }
                else
                {
                    ival = va_arg(ap, int32_t);
                }
            }

            {
                char buff[22];
                const int base = (*format == 'x' || *format == 'p') ? 16 : 10;

                /* *INDENT-OFF* */
                char *val_as_str = (*format == 'u') ? _safe_utoa(base, uval, &buff[sizeof(buff) - 1]) : _safe_itoa(base, ival, &buff[sizeof(buff) - 1]);
                /* *INDENT-ON* */

                /* Strip off "ffffffff" if we have 'x' format without 'll' */
                if (*format == 'x' && !have_longlong && ival < 0)
                {
                    val_as_str += 8;
                }

                while (*val_as_str && to < end)
                {
                    *to++ = *val_as_str++;
                }
                continue;
            }
        }
        case 's':
        {
            const char *val = va_arg(ap, char *);
            if (!val)
            {
                val = "(null)";
            }
            while (*val && to < end)
            {
                *to++ = *val++;
            }
            continue;
        }
        }
    }
    *to = 0;
    return (int)(to - start);
}

int _safe_snprintf(char *to, size_t n, const char *fmt, ...)
{
    int result;
    va_list args;
    va_start(args, fmt);
    result = _safe_vsnprintf(to, n, fmt, args);
    va_end(args);
    return result;
}

void log_deinit(void)
{
    struct logger *l = &logger;

    if (l->fd < 0 || l->fd == STDERR_FILENO)
    {
        return;
    }

    close(l->fd);
}

void log_reopen(void)
{
    struct logger *l = &logger;

    if (l->fd != STDERR_FILENO)
    {
        close(l->fd);
        l->fd = open(l->name, O_WRONLY | O_APPEND | O_CREAT, 0644);
        if (l->fd < 0)
        {
            log_stderr_safe("reopening log file '%s' failed, ignored: %s", l->name,
                            strerror(errno));
        }
    }
}

void log_level_up(void)
{
    struct logger *l = &logger;

    if (l->level < LOG_TRACE)
    {
        l->level++;
        log_safe("up log level to %d", l->level);
    }
}

void log_level_down(void)
{
    struct logger *l = &logger;

    if (l->level > LOG_ERR)
    {
        l->level--;
        log_safe("down log level to %d", l->level);
    }
}

void log_level_set(int level)
{
    struct logger *l = &logger;

    l->level = MAX(LOG_ERR, MIN(level, LOG_TRACE));
    log_safe("set log level to %d", l->level);
}

void _log(const char *file, int line, int panic, const char *fmt, ...)
{
    struct logger *l = &logger;
    int len, size, errno_save;
    char buf[LOG_MAX_LEN];
    va_list args;
    ssize_t n;
    struct timeval tv;

    if (l->fd < 0)
    {
        return;
    }

    errno_save = errno;
    len = 0;            /* length of output buffer */
    size = LOG_MAX_LEN; /* size of output buffer */

    gettimeofday(&tv, NULL);
    buf[len++] = '[';
    len += _log_strftime(buf + len, size - len, "%Y-%m-%d %H:%M:%S.", localtime(&tv.tv_sec));
    len += _log_scnprintf(buf + len, size - len, "%03ld", tv.tv_usec / 1000);
    // len += _log_scnprintf(buf + len, size - len, "] [%s] %s:%d ", logger_level_names[l->level], file, line);
    len += _log_scnprintf(buf + len, size - len, "] [%s%s\x1b[0m \x1b[90m] %s:%d ", logger_level_colors[l->level], logger_level_names[l->level], file, line);

    va_start(args, fmt);
    len += _log_vscnprintf(buf + len, size - len, fmt, args);
    va_end(args);

    buf[len++] = '\n';

    n = _log_write(l->fd, buf, len);
    if (n < 0)
    {
        l->nerror++;
    }

    errno = errno_save;

    if (panic)
    {
        abort();
    }
}
