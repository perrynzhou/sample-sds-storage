/*************************************************************************
  > File Name: log.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Sun 24 Nov 2019 10:52:54 AM CST
 ************************************************************************/

#ifndef _LOG_H_
#define _LOG_H_

struct logger
{
    char *name; /* log file name */
    int level;  /* log level */
    int fd;     /* log file descriptor */
    int nerror; /* # log error */
};

enum logger_level
{
    LOG_ERR = 0,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG,
    LOG_TRACE,
};

enum logger_output_type
{
    LOG_STDOUT_TYPE = 0,
    LOG_STDERR_TYPE = 1,
    LOG_DEFINE_TYPE,
};
#define LOG_MAX_LEN (4096) /* max length of log message */

#define log_safe(...)                                  \
    do                                                 \
    {                                                  \
        _log_safe(__FILE__, __LINE__, 0, __VA_ARGS__); \
    } while (0)
#define log(...)                                  \
    do                                            \
    {                                             \
        _log(__FILE__, __LINE__, 0, __VA_ARGS__); \
    } while (0)
int log_init(int level, int output_type, char *filename);
void log_deinit(void);
void log_level_up(void);
void log_level_down(void);
void log_level_set(int level);
void log_reopen(void);
void _log(const char *file, int line, int panic, const char *fmt, ...);
void _log_safe(const char *file, int line, int panic, const char *fmt, ...);

#endif
