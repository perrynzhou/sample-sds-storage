/*************************************************************************
  > File Name: utils.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 07:15:54 PM CST
 ************************************************************************/

#ifndef _UTILS_H
#define _UTILS_H
#include "slice.h"
#include <unistd.h>
ssize_t read_file(int fd, void *buf, size_t count);
ssize_t write_file(int src_fd, int dst_fd,const void *buf, size_t count);
int  parse_filename_from_path(slice *se,const char *path);
#endif
