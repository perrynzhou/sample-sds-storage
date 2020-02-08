/*************************************************************************
  > File Name: bucket.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 10:23:16 AM CST
 ************************************************************************/

#include "bucket.h"
#include "bucket_item.h"
#include "bucket_store.h"
#include "list.h"
#include "hash.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#define BUCKET_LIST_SIZE (4)
bucket *bucket_create(const char *bucket_prefix_name, uint64_t block_size, uint64_t start, uint64_t end)
{
  bucket *bt = NULL;
  if (bucket_prefix_name != NULL)
  {
    bt = (bucket *)calloc(1, sizeof(bucket));
    assert(bt != NULL);
    bt->index = 0;
    bt->block_size = block_size;
    vector_init(&bt->bucket_items, end - start, NULL);
    char active_path[256] = {'\0'};
    bucket_store_init(&bt->bs);
    slice_init(&bt->bucket_prefix_name, bucket_prefix_name);
    bt->min_rank = start;
    bt->max_rank = end;
  }
  return bt;
}
int bucket_put(bucket *bt, bucket_object *obj)
{
  int ret = -1;

  if (bt != NULL || obj != NULL)
  {

    bucket_item *cur_item = vector_at(&bt->bucket_items, obj->hash);
    if (cur_item == NULL)
    {
      cur_item = bucket_item_create(slice_value(&bt->bucket_prefix_name),obj->hash,bt->index,bt->block_size);
      slice_init(&cur_item->bukcet_item_prefix_name,slice_value(&bt->bucket_prefix_name));
      vector_insert(&bt->bucket_items, obj->hash, cur_item);
    }
     __sync_fetch_and_add(&cur_item->used_bytes,obj->data_len);
    list_node *node = list_node_create(obj);
    assert(node != NULL);
    ret = list_add(cur_item->objects, node);
    bucket_item_store(cur_item,obj,&bt->index);
    return 0; 
  }
  return ret;
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