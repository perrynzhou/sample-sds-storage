/*************************************************************************
  > File Name: bucket.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 10:23:16 AM CST
 ************************************************************************/

#include "bucket.h"
#include "bucket_store.h"
#include "list.h"
#include "hash.h"
#include "log.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#define BUCKET_LIST_SIZE (4)
static int bucket_create_data_fd(bucket *bt)
{
  int fd = -1;
  char buffer[4096] = {'\0'};
  snprintf((char *)&buffer, 4096, "%s.%d", slice_value(&bt->name), bt->index);
  char *file = (char *)&buffer;
  struct stat st;
  if (stat(file, &st) == -1)
  {
    fd = open(file, O_CREAT | O_RDWR | O_APPEND);
    if (fallocate(fd, 0, 0, bt->data_file_max_length) != 0)
    {
      log_fatal_safe("fallocate file %s,failed:%s", file, strerror(errno));
    }
    log_info("create file %s success,length:%d mb", file, bt->data_file_max_length / 1024 / 1024);
  }
  return fd;
}
bucket *bucket_create(const char *name, uint64_t data_file_max_length, uint64_t start, uint64_t end)
{
  bucket *bt = NULL;
  if (name != NULL)
  {
    bt = (bucket *)calloc(1, sizeof(bucket));
    assert(bt != NULL);
    bt->index = 0;
    bt->data_file_max_length = data_file_max_length;
    bt->data_file_cur_length = 0;
    bucket_store_init(&bt->bs);
    slice_init(&bt->name, name);
    bt->min_rank = start;
    bt->max_rank = end;
    bt->fd = bucket_create_data_fd(bt);
    vector_init(&bt->cache, bt->max_rank - bt->min_rank, NULL);
    log_info_safe("init bucket %d cache,each contains %ld list", bt->max_rank - bt->min_rank);
    pthread_mutex_init(&bt->lock, NULL);
    log_info_safe("init bucket success");
  }

  return bt;
}

static int bucket_check_data_fd(bucket *bt)
{

  if (bt->data_file_cur_length >= bt->data_file_max_length)
  {
    pthread_mutex_lock(&bt->lock);
    if (bt->fd != -1)
    {
      close(bt->fd);
    }
    int fd = bucket_create_data_fd(bt);
    if (fd == -1)
    {
      log_err_safe("switch data file to %s.%d failed:%s", slice_value(&bt->name), bt->index + 1);
      return -1;
    }
    bt->fd = fd;
    bt->index++;
    pthread_mutex_unlock(&bt->lock);
  }
  log_info_safe("open data file:%s.%d success,fd =%d", slice_value(&bt->name), bt->index, bt->fd);
  return 0;
}
int bucket_put(bucket *bt, bucket_object *obj)
{

  if (bt != NULL || obj != NULL)
  {
    if (bucket_check_data_fd(bt) != 0)
    {
      log_fatal("%s.%d can't switch new file", slice_value(&bt->name), bt->index);
    }
    uint32_t index = hash_jump_consistent(obj->obj_hash, (bt->max_rank-bt->min_rank))+bt->min_rank;
    log_info_safe("current bucket-%d ,insert index:%ld,start:%ld,end:%d",bt->id,index,bt->min_rank,bt->max_rank);
    list *lt = vector_at(&bt->cache,index);
    if (lt == NULL)
    {
      lt = list_create();
      assert(lt != NULL);
      vector_insert(&bt->cache, obj->obj_hash, lt);
    }
    list_node *node = list_node_create(obj);
    list_add(lt, node);
    bt->bs.write_bucket_object(bt, obj);
    return 0;
  }
  return -1;
}
int bucket_del(bucket *bt, const char *name)
{
  return 0;
}
bucket_object *bucket_get(bucket *bt, const char *name)
{
  return NULL;
}
void bucket_destroy(bucket *bt)
{
}