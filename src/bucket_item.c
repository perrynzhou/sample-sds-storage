/*************************************************************************
  > File Name: bucket_item.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 03:45:42 PM CST
 ************************************************************************/
 #define _GNU_SOURCE             /* See feature_test_macros(7) */
#include "bucket_item.h"
#include "bucket_store.h"
#include "bucket_object.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

bucket_item *bucket_item_create(const char *path, uint64_t hash, uint64_t bucket_index, uint64_t block_size)
{
  bucket_item *item = NULL;
  if (path != NULL)
  {
    log_info_safe("start new path:%s,hash:%ld,bucket_index :%ld,block_size:%ld", path, hash, bucket_index, block_size);
    item = (bucket_item *)calloc(1, sizeof(bucket_item));
    assert(item != NULL);
    item->bucket_index = bucket_index;
    item->hash = hash;
    item->objects = list_create();
    assert(item->objects != NULL);
    char active_path[256] = {'\0'};
    snprintf((char *)&active_path, 256, "%s.%d", path, bucket_index);
    int fd = open((char *)&active_path, O_RDWR | O_CREAT | O_APPEND);
    assert(fd !=0);
    assert(fallocate(fd,FALLOC_FL_KEEP_SIZE,0,block_size)!=0);
    item->fd = fd;
    item->block_size = block_size;
    item->used_bytes = 0;
  }
  return item;
}
int bucket_item_store(bucket_item *item,bucket_object *obj,uint64_t *bucket_index)
{
  int ret = -1;
  if (item->used_bytes >= item->block_size)
  {
    close(item->fd);
     __sync_fetch_and_add(&item->bucket_index,1);
    char active_path[256] = {'\0'};
    snprintf((char *)&active_path, 256, "%s.%d", slice_value(&item->bukcet_item_prefix_name), item->bucket_index);
    int fd = open((char *)&active_path, O_RDWR | O_CREAT | O_APPEND);
    assert(fallocate(fd,FALLOC_FL_KEEP_SIZE,0,item->block_size)!=0);
    item->fd = fd;
     __sync_fetch_and_add(&bucket_index,1);
  }
  //obj->bucket_index = item->bucket_index;
  ret =item->store.write_bucket_object(item,obj);
  log_debug_safe("write object size %d bytes,ret =%d",obj->data_len,ret);
  //log("####write object size %d bytes,ret =%d",obj->data_len,ret);

  return ret;
 
}
void bucket_item_destroy(bucket_item *item)
{
}