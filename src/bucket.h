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
#define BUCKET_DEFAULT_OBJECT_SIZE (16384)
#define BUCKET_PREFIX_FMT  ("%s-%ld-%ld-bucket")
typedef struct bucket_t {
  slice bucket_prefix_name; 
  uint64_t min_rank;
  uint64_t max_rank;
  uint64_t block_size;//each block as data_file
  uint64_t index;
  bucket_store bs;
  vector bucket_items;//save bucket_item,each list save 16384 bucket_item
  void *ctx;
}bucket;

bucket *bucket_create(const char *bucket_prefix_name,uint64_t block_size,uint64_t start,uint64_t end);
int bucket_put(bucket *bt,bucket_object *obj);
int bucket_del(bucket *bt,const char *name);
bucket_object *bucket_get(bucket *bt,const char *name);
void bucket_destroy(bucket *bt);


#endif
