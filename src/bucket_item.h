/*************************************************************************
  > File Name: bucket_item.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 03:45:35 PM CST
 ************************************************************************/

#ifndef _BUCKET_ITEM_H
#define _BUCKET_ITEM_H
#include "list.h"
#include "slice.h"
#include "bucket_store.h"
#include "bucket_object.h"
#include <stdint.h>
typedef struct  bucket_item_t {
   int fd;
   slice  bukcet_item_prefix_name;
   uint64_t used_bytes;
   uint64_t block_size;
   uint64_t hash;
   uint64_t bucket_index;
   list *objects; //save object
   bucket_store store;
   void *ctx;
}bucket_item;
bucket_item *bucket_item_create(const char *path,uint64_t hash,uint64_t block_size,uint64_t bucket_index);
int bucket_item_store(bucket_item *item,bucket_object *obj,uint64_t *flag);
void bucket_item_destroy(bucket_item *item);
#endif
