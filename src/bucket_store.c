/*************************************************************************
  > File Name: bucket_store.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 02:24:22 PM CST
 ************************************************************************/

#include "bucket_store.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
ssize_t read_bucket_item(void *bucet_item)
{
}
ssize_t write_bucket_item(void *bucket_item)
{
}
ssize_t read_bucket_object(void *bucket, void *object)
{
}
ssize_t write_bucket_object(void *bucket, void *object)
{
}
int delete_bucket_item(int fd, void *bucket)
{
}
int bucket_store_init(bucket_store *bs)
{
  int ret = -1;
  if (bs != NULL)
  {
    bs->read_bucket_item = read_bucket_item;
    bs->write_bucket_item = write_bucket_item;
    bs->read_bucket_object = read_bucket_object;
    bs->write_bucket_object = write_bucket_object;
    ret = 0;
    log_err_safe("log-safe init bucket_store  functions success");
    log_info("#####log init bucket_store  functions success");

  }
  return ret;
}
void bucket_store_deinit(bucket_store *bs)
{
  if (bs != NULL)
  {
    bs->read_bucket_item = NULL;
    bs->write_bucket_item = NULL;
    bs->read_bucket_object = NULL;
    bs->write_bucket_object = NULL;
    log_info_safe("bucket store  deinit success");
  }
}