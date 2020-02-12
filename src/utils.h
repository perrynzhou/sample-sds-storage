/*************************************************************************
  > File Name: utils.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 07:15:54 PM CST
 ************************************************************************/

#ifndef _UTILS_H
#define _UTILS_H
#include <unistd.h>
ssize_t write_n(int fd, const void *buf, size_t len);
ssize_t read_n(int fd, void *buf, size_t len);
void fetch_sock_addr_info(int fd, char *str);
#endif
