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
      fprintf(stdout, "write error: %s(errno: %d)\n", strerror(errno), errno);
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
int parse_file_name(slice *se, const char *path)
{
  if (se != NULL && path != NULL)
  {
    char *base = (char *)path;
    size_t len = strlen(base);
    int index = 0, pos = 0;
    while (base[index] != '\0')
    {
      if (base[index] == '/')
      {
        pos = index + 1;
      }
      index++;
    }
    slice_init(se, path + pos);
    return 0;
  }
  return -1;
}
#ifdef UTILS_TEST
int main(void)
{
  slice se1;
  slice se2;
  parse_file_name(&se1, "./a/b.txt");
  parse_file_name(&se2, "acb.txt");
  slice_print(&se1);
  slice_print(&se2);
  return 0;
}
#endif