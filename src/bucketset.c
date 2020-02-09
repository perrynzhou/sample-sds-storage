/*************************************************************************
  > File Name: bucketset.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 10:31:01 AM CST
 ************************************************************************/

#include "bucketset.h"
#include "bucket.h"
#include "hash.h"
#include "log.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#define BUCKET_PREFIX_FMT ("%s-%ld-%ld-bucket")
#define BUCKET_MIN_HASH (512)
static uint64_t bucketset_hash_range_round(uint64_t base)
{
  int i = 1;
  uint64_t value = 0;
  uint32_t n = 16384;
  while (i > 0)
  {
    uint64_t hash = i * BUCKET_MIN_HASH;
    if (hash > base)
    {
      value = hash;
      break;
    }
    i++;
  }
  return value;
}
bucketset *bucketset_create(const char *name, const char *data_dir, uint64_t bucket_size, uint64_t bucket_range_count, uint64_t data_file_size_unit)
{
  bucketset *bst = (bucketset *)calloc(1, sizeof(bucketset));
  assert(bst != NULL);
  bst->bucket_size = bucket_size;
  bst->bucket_range_count = bucketset_hash_range_round(bucket_range_count);
  vector_cb cb = (vector_cb)&bucket_destroy;
  int ret = vector_init(&bst->buckets, bst->bucket_size, cb);
  log_info_safe("init bucketset[%p] ,ret %d,buckets[%p],cap %ld", bst, ret, &bst->buckets, bst->buckets.cap);
  bst->data_file_size_unit = data_file_size_unit;
  slice_init(&bst->data_dir, data_dir);
  slice_init(&bst->set_name, name);
  for (uint64_t i = 0; i < bst->bucket_size; i++)
  {
    uint64_t start_offset = i * bst->bucket_range_count;
    uint64_t end_offset = (i + 1) * bst->bucket_range_count - 1;
    char buffer[4096] = {'\0'};
    snprintf((char *)&buffer, 4096, "%s/%s-%ld-%ld-bucket", data_dir, name, start_offset, end_offset);
    bucket *bt = bucket_create((char *)&buffer, bst->data_file_size_unit * 1024 * 1024, start_offset, end_offset);
    assert(bt != NULL);
    bt->id = i;
    log_info("create bucket[%p],start_offset %ld,end_offset %ld", bt, bt->min_rank, bt->max_rank);
    ret = vector_push_back(&bst->buckets, bt);
  }
  log_info_safe("init bucket data file length,base unit is  %d Mb", data_file_size_unit);
  slice_init(&bst->set_name, name);
  bucketset_dump(bst);
  return bst;
}
void bucketset_print(bucketset *bst)
{
  if (bst != NULL)
  {
    fprintf(stdout, "bucketset_name:%s\n", slice_value(&bst->set_name));
    fprintf(stdout, "data_file_size :%d Mb\n", bst->data_file_size_unit);
    fprintf(stdout, "nucket_size :%d\n", bst->bucket_size);
    fprintf(stdout, "bucket_range_count:%ld\n", bst->bucket_range_count);
    fprintf(stdout, "bucket info:\n");
    for (uint32_t i = 0; i < bst->bucket_size; i++)
    {
      bucket *bt = (bucket *)vector_at(&bst->buckets, i);
      fprintf(stdout, "bucket[%d]:name=%s.%d,min=%ld,max=%ld\n", i, slice_value(&bt->name), bt->index, bt->min_rank, bt->max_rank);
    }
  }
}
int bucketset_dump(bucketset *bst)
{
  return -1;
}
bucket *bucketset_search_bucket(bucketset *bst, uint64_t hash)
{
  bucket *bkt = NULL;
  uint32_t h = hash_jump_consistent(hash, bst->bucket_size);
  for (int i = 0; i < bst->buckets.size; i++)
  {
    bucket *bt = (bucket *)vector_at(&bst->buckets, i);
    if (hash >= bt->min_rank && hash < bt->max_rank)
    {
      log_info_safe("find target bucket,min_rank=%ld,max_rank=%ld", bt->min_rank, bt->max_rank);
      bkt = bt;
      break;
    }
  }
  return bkt;
}
void bucketset_destroy(bucketset *bt)
{
}
int bucketset_delete_bucket(bucketset *bst, bucket *bt)
{
}
#ifdef BUCKETSET_TEST
int main(void)
{
  log_init(LOG_STDOUT_TYPE, NULL);
  bucketset *bst = bucketset_create("test_set", "/tmp", 3, 8192, 64);
  bucketset_print(bst);
  return 0;
}
#endif