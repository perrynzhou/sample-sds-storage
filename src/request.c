/*************************************************************************
  > File Name: request.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Wed 12 Feb 2020 01:51:40 PM CST
 ************************************************************************/

#include "request.h"
#include "utils.h"
#include "md5.h"
#include "slice.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#define REQUEST_READ_BUFFER_LENGTH (1024 * 1024)
int request_put_object_init(int sock, const char *path)
{
  if (sock != -1 && path != NULL)
  {
    struct stat st;
    if (stat(path, &st) != -1)
    {
      size_t req_len = REQUEST_READ_BUFFER_LENGTH + sizeof(request_put_object);
      request_put_object *req = (request_put_object *)calloc(req_len, sizeof(char));
      assert(req != NULL);
      uint8_t uid[BUCKET_OBJECT_UID_SZ] = {'\0'};
      slice name;
      parse_file_name(&name, path);
      req->data_length = st.st_size;
      srand((int)time(NULL));
      req->req_id = rand();
      req->req_type = REQ_PUT_OBJECT;
      strncpy((char *)&req->name, slice_value(&name), slice_size(&name));
      md5_file(path, (char *)&uid);
      slice_deinit(&name);
      size_t length = req->data_length;
      int fd = open(path, O_RDONLY);
      size_t rn = 0;
      while (rn < length)
      {
        size_t n = read_n(fd, (char *)req->data, REQUEST_READ_BUFFER_LENGTH);
        rn += n;
        if (write_n(sock, req, req_len) != req_len)
        {
          fprintf(stdout, "write failed\n");
          break;
        }
      }
      close(sock);
      return 0;
    }
  }
  return -1;
}