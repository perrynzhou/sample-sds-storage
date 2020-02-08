/*************************************************************************
  > File Name: utils.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 07:16:00 PM CST
 ************************************************************************/

#include "utils.h"
#include <stdio.h>
#include <string.h>
ssize_t read_file(int fd, void *buf, size_t count)
{
  return 0;
}
ssize_t write_file(int fd,int dst_fd,const void *buf, size_t count)
{
  return 0;
}
int parse_filename_from_path(slice *se, const char *path)
{
  int ret = -1;
  if (!se || !path)
  {
    return ret;
  }
  size_t len = strlen(path);
  const char *base = path;
  if (strchr(path, '/') == NULL)
  {
    slice_strncpy(se, path, len);
    return ret;
  }
  for (int i = len - 1; i >= 0; i--)
  {
    if (base[i] == '/')
    {
      slice_strncpy(se, base + i + 1, len - i);
      ret = 0;
      break;
    }
  }
  return ret;
}
#ifdef UTILS_TEST
int main(void)
{
  const char *a = "aaa.txt";
  const char *b = "/a/c/b";
  slice sa;
  slice sb;
  parse_filename_from_path(&sa, a);
  parse_filename_from_path(&sb, b);
  slice_print(&sa);
  slice_print(&sb);
  return 0;
}
#endif