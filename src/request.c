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
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#define REQUEST_READ_BUFFER_LENGTH (4*1024 * 1024)
int  request_put_object_init(response_ack *ack,int sock,const char *path,const char *bucket_name)
{
  bool is_success = false;
  request_put_object *req = NULL;
        char *data = NULL;
  if (sock != -1 && path != NULL)
  {
    struct stat st;
    if (stat(path, &st) != -1)
    {
      req = (request_put_object *)calloc(1, sizeof(request_put_object));
      assert(req != NULL);
      uint8_t req_type = REQ_PUT_OBJECT;
      write_n(sock,&req_type,sizeof(uint8_t));

      uint8_t uid[BUCKET_OBJECT_UID_SZ] = {'\0'};
      slice name;
      parse_file_name(&name, path);
      req->data_length = st.st_size;
      strncpy((char *)&req->object_name, slice_value(&name), slice_size(&name));
      strncpy((char *)&req->bucket_name, bucket_name, strlen(bucket_name));
      md5_file(path, (char *)&uid);
      slice_deinit(&name);
      size_t length = req->data_length;
      write_n(sock,req,sizeof(*req));

      int fd = open(path, O_RDONLY);
      size_t rn = 0;
       data = (char *)calloc(REQUEST_READ_BUFFER_LENGTH,sizeof(char));
      assert(data!=NULL);
      while (rn < length)
      {
        size_t n = read_n(fd, data, REQUEST_READ_BUFFER_LENGTH);
        rn += n;
        ssize_t ret = write_n(sock, req, REQUEST_READ_BUFFER_LENGTH);
        if (ret <0)
        {
          fprintf(stdout, "write failed\n");
          is_success = false;
          goto out;
        }else if(ret==0) {
          is_success = true;
          break;
        }
      }
      if (is_success)
      {
        read_n(fd, ack, sizeof(*ack));
      }
    }
  }
out:
  if (req != NULL)
  {
    free(req);
  }
  if(data!=NULL) {
    free(data);
  }
  if (sock != -1)
  {
    close(sock);
  }
  return (is_success) ? 0 : -1;
}
int request_put_bucketset_init(response_ack *ack, int sock, const char *set_name,const char *bucket_name)
{
  bool is_success = false;
  request_put_bucketset *req = NULL;
  if (sock != -1 && set_name != NULL)
  {
    uint8_t req_type = REQ_PUT_BUCKETSET;
    write_n(sock,&req_type,sizeof(uint8_t));
    req = (request_put_bucketset *)calloc(1, sizeof(request_put_bucketset));
    assert(req != NULL);
    strncpy((char *)&req->set_name, set_name, strlen(set_name));
    strncpy((char *)&req->bucket_name, bucket_name, strlen(bucket_name));

    if (read_n(sock, req, sizeof(*req)) != sizeof(*req))
    {
      is_success = false;
      goto out;
    }
    is_success = true;
    if (is_success)
    {
      read_n(sock, ack, sizeof(*ack));
    }
  }
out:
  if (req != NULL)
  {
    free(req);
  }
  if (sock != -1)
  {
    close(sock);
  }
  return (is_success) ? 0 : -1;
}