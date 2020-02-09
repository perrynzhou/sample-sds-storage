/*************************************************************************
  > File Name: bucket.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 10:23:09 AM CST
 ************************************************************************/

#ifndef _BUCKET_H
#define _BUCKET_H
#include "slice.h"
#include "list.h"
#include "bucket_object.h"
#include "vector.h"
#include "bucket_store.h"
#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#define BUCKET_DEFAULT_OBJECT_SIZE (16384)
typedef struct bucket_t {
  int     fd;//save data file fd
  int      id;
  slice    name; 
  uint64_t min_rank;
  uint64_t max_rank;
  uint64_t data_file_max_length;//data file length
  uint64_t data_file_cur_length;
  uint64_t index;
  pthread_mutex_t lock;
  bucket_store bs;
  vector   cache;//hashtable,each element save a list
  void *ctx;
}bucket;

bucket *bucket_create(const char *name,uint64_t data_file_max_length,uint64_t start,uint64_t end);
int bucket_put(bucket *bt,bucket_object *obj);
int bucket_del(bucket *bt,const char *name);
bucket_object *bucket_get(bucket *bt,const char *name);
void bucket_destroy(bucket *bt);


#endif
