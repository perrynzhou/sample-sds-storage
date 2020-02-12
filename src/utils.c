/*************************************************************************
  > File Name: utils.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 07:16:00 PM CST
 ************************************************************************/

#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
void fetch_sock_addr_info(int fd, char *str)
{
  struct sockaddr ip_addr_struct;
  socklen_t len = sizeof(ip_addr_struct);
  getpeername(fd, (struct sockaddr *)&ip_addr_struct, &len);
  struct sockaddr_in *s = (struct sockaddr_in *)&ip_addr_struct;
  int port = ntohs(s->sin_port);
  char ip_addr[64] = {'\0'};
  inet_ntop(AF_INET, &s->sin_addr, (char *)&ip_addr, 64);
  sprintf(str, "%s:%d", ip_addr, port);
}
ssize_t write_n(int fd, const void *buf, size_t len)
{
  int writen = 0;
  while (writen < len)
  {
    ssize_t w = write(fd, (const char *)buf + writen, len - writen);
    if (w > 0)
    {
      writen += w;
    }
    else if (w == 0)
    {
      break;
    }
    else if (errno != EINTR)
    {
      fprintf(stdout,"write error: %s(errno: %d)\n", strerror(errno), errno);
      break;
    }
  }
  return writen;
}
ssize_t read_n(int fd, void *buf, size_t len)
{
  int readn = 0;
  while (readn < len)
  {
    ssize_t r = read(fd, (char *)buf + readn, len - readn);
    if (r > 0)
    {
      readn += r;
    }
    else if (r == 0)
    {
      break;
    }
    else if (r != EINTR)
    {
      printf("read error: %s(errno: %d)\n", strerror(errno), errno);
      break;
    }
  }
  return readn;
}
#ifdef UTILS_TEST
int main(void)
{
  return 0;
}
#endif