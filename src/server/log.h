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
    int fd;     /* log file descriptor */
    int nerror; /* # log error */
};

enum logger_level
{
    LOG_FATAL = 0,
    LOG_ERR,
    LOG_WARN,
    LOG_INFO,
    LOG_DEBUG
};

enum logger_output_type
{
    LOG_STDOUT_TYPE = 0,
    LOG_STDERR_TYPE = 1,
    LOG_DEFINE_TYPE,
};
#define log_info(...)                                    \
    do                                                   \
    {                                                    \
        _log(__FILE__, __LINE__, LOG_INFO, __VA_ARGS__); \
    } while (0)
#define log_fatal(...)                                    \
    do                                                    \
    {                                                     \
        _log(__FILE__, __LINE__, LOG_FATAL, __VA_ARGS__); \
    } while (0)
#define log_err(...)                                    \
    do                                                  \
    {                                                   \
        _log(__FILE__, __LINE__, LOG_ERR, __VA_ARGS__); \
    } while (0)
#define log_warn(...)                                    \
    do                                                   \
    {                                                    \
        _log(__FILE__, __LINE__, LOG_WARN, __VA_ARGS__); \
    } while (0)
#define log_debug(...)                                    \
    do                                                    \
    {                                                     \
        _log(__FILE__, __LINE__, LOG_DEBUG, __VA_ARGS__); \
    } while (0)

#define log_info_safe(...)                                    \
    do                                                        \
    {                                                         \
        _log_safe(__FILE__, __LINE__, LOG_INFO, __VA_ARGS__); \
    } while (0)
#define log_fatal_safe(...)                                    \
    do                                                         \
    {                                                          \
        _log_safe(__FILE__, __LINE__, LOG_FATAL, __VA_ARGS__); \
    } while (0)
#define log_err_safe(...)                                    \
    do                                                       \
    {                                                        \
        _log_safe(__FILE__, __LINE__, LOG_ERR, __VA_ARGS__); \
    } while (0)
#define log_warn_safe(...)                                    \
    do                                                        \
    {                                                         \
        _log_safe(__FILE__, __LINE__, LOG_WARN, __VA_ARGS__); \
    } while (0)
#define log_debug_safe(...)                                    \
    do                                                         \
    {                                                          \
        _log_safe(__FILE__, __LINE__, LOG_DEBUG, __VA_ARGS__); \
    } while (0)
int log_init(int output_type, char *filename);
void log_deinit(void);
void log_reopen(void);
void _log(const char *file, int line, int level, const char *fmt, ...);
void _log_safe(const char *file, int line, int level, const char *fmt, ...);

#endif
