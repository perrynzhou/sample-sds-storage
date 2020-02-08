/*************************************************************************
  > File Name: bucket_store.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 02:24:17 PM CST
 ************************************************************************/

#ifndef _BUCKET_STORE_H
#define _BUCKET_STORE_H
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
typedef struct bucket_store_t
{
  ssize_t (*read_bucket_item)(void *bucet_item);
  ssize_t (*write_bucket_item)(void *bucket_item);
  ssize_t (*read_bucket_object)(void *bucket, void *object);
  ssize_t (*write_bucket_object)(void *bucket, void *object);
  int (*delete_bucket_item)(int fd, void *bucket);
} bucket_store;
int bucket_store_init(bucket_store *bs);
void bucket_store_deinit(bucket_store *bs);
#endif
