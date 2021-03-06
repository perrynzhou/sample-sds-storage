/*************************************************************************
  > File Name: bucketset.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 10:30:56 AM CST
 ************************************************************************/

#ifndef _BUCKETSET_H
#define _BUCKETSET_H
#include "bucket.h"
#include "vector.h"
#include <stdint.h>
typedef struct bucketset_t
{
  slice data_dir;
  slice set_name;
  uint64_t bucket_object_size;
  uint64_t bucket_range_count;
  uint64_t bucket_size;
  vector buckets;               // vector save bucket;
  uint64_t data_file_size_unit; //mb as unit
} bucketset;
bucketset *bucketset_create(const char *name, const char *data_dir, uint64_t bucket_size, uint64_t bucket_range_count, uint64_t data_file_size_unit);
int bucketset_delete_bucket(bucketset *bst, bucket *bt);
void bucketset_print(bucketset *bst);
int bucketset_dump(bucketset *bst);
bucket *bucketset_search_bucket(bucketset *bt, uint64_t hash);
void bucketset_destroy(bucketset *bt);
#endif
