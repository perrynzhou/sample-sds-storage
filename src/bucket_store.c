/*************************************************************************
  > File Name: bucket_store.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 02:24:22 PM CST
 ************************************************************************/

#include "bucket_store.h"
#include "log.h"
#include "bucket.h"
#include "bucket_object.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
int read_bucket_object(void *bucket, void *object)
{
}
int write_bucket_object(void *bt, void *arg,int fd)
{
  bucket *bkt = (bucket *)bt;
  bucket_object *obj=(bucket_object *)arg;
  if(!bt||!obj) {
     return -1;
  }
  ssize_t  len = obj->data_len;
  ssize_t   read_len =0;
  ssize_t   write_len=0;
  size_t buffer_len = 4*1024*1024;
  char *buffer = NULL;
  int ret = -1;
  while(read_len<len){
    ssize_t  rn = read_n(fd,buffer,len);
    if(rn>0) {
      ssize_t wn=write_n(bkt->fd,buffer,rn);
      read_len+=rn;
    }
    if(rn<=0) {
      ret = 0;
      break;
    }
  }
  if(write_len != obj->data_len) {
    goto out;
  }
out:
  if(buffer!=NULL) {
    free(buffer);
  }
  return ret;

}
int delete_bucket_object(int fd, void *bucket)
{
}
int bucket_store_init(bucket_store *bs)
{
  int ret = -1;
  if (bs != NULL)
  {
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
    bs->read_bucket_object = NULL;
    bs->write_bucket_object = NULL;
    log_info_safe("bucket store  deinit success");
  }
}