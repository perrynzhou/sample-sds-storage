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
#define BUCKETSET_EACH_BUCKET_SIZE  (64)
#define BUCKET_DEFAULT_BLOCK_SIZE  (64*1024*1024)
bucketset *bucketset_create(const char *name, uint64_t bucket_group_size,uint64_t bucket_max_size, uint64_t block_size)
{
  bucketset *bst = (bucketset *)calloc(1, sizeof(bucketset));
  assert(bst != NULL);
  int ret = -1;
  bst->bucket_cur_size = 0;
  bst->bucket_group_size = bucket_group_size;
  bst->bucket_max_size = (bucket_max_size<BUCKETSET_EACH_BUCKET_SIZE)? BUCKETSET_EACH_BUCKET_SIZE:bucket_max_size;
  bst->bucket_max_hash = bst->bucket_max_size*bst->bucket_group_size;
  vector_cb cb = (vector_cb)&bucket_destroy;
  ret = vector_init(&bst->buckets, bst->bucket_max_hash, cb);
  log_safe("init bucketset %p ,ret %d,bucketset vector %p,cap %ld", bst, ret, &bst->buckets, bst->buckets.cap);
  for (uint64_t i = 0; i < bst->bucket_group_size; i++)
  {
    uint64_t start_offset = i * BUCKET_DEFAULT_OBJECT_SIZE;
    uint64_t end_offset  = (i+1)*BUCKET_DEFAULT_OBJECT_SIZE-1;
    slice *bucket_prefix_name = slice_create_with_fmt(BUCKET_PREFIX_NAME, name, start_offset, end_offset);
    bucket *bt = bucket_create(slice_value(bucket_prefix_name), block_size, start_offset, end_offset);
    assert(bt != NULL);
    log_safe("bucket %p,start_offset %ld,end_offset %ld", bt, bt->min_rank, bt->max_rank);
    ret = vector_push_back(&bst->buckets, bt);
    log_safe("add bucket %p to vector %p,ret %d,vector size %ld", &bst, &bst->buckets, ret, bst->buckets.size);
    slice_destroy(bucket_prefix_name);
  }
  bst->block_size =block_size*1024*1024;
  log_safe("default block size %d Mb", block_size);
  slice_init(&bst->set_name, name);
  bucketset_dump(bst);
  return bst;
}
void bucketset_print(bucketset *bst){
  if(bst !=NULL) {
    fprintf(stdout,"bucketset_name:%s\n",slice_value(&bst->set_name));
    fprintf(stdout,"block_size :%d Mb\n",bst->block_size/1024/1024);
    fprintf(stdout,"bucket_max_hash:%d\n",bst->bucket_max_hash);
    fprintf(stdout,"bucket_cur_size:%d\n",bst->bucket_cur_size);
    fprintf(stdout,"bucket_hash:%ld\n",bst->bucket_max_hash);
    fprintf(stdout,"bucket_max_size:%ld\n",bst->bucket_max_size);
    fprintf(stdout,"bucket info:\n");
    for(uint32_t i=0;i<bst->bucket_group_size;i++) {
      bucket *bt = (bucket *)vector_at(&bst->buckets,i);
      fprintf(stdout,"bucket[%d]:min=%ld,max=%ld\n",i,bt->min_rank,bt->max_rank);
    }
  }
}
int bucketset_dump(bucketset *bst){
  return -1;
}
bucket *bucketset_search_bucket(bucketset *bst, uint64_t hash)
{
  uint32_t h = hash_jump_consistent(hash,bst->bucket_max_size);
  log_safe("vector cap size %ld,search index %ld",bst->buckets.size,h);
  return (bucket *)vector_at(&bst->buckets,h);
}
void bucketset_destroy(bucketset *bt)
{
}
int bucketset_delete_bucket(bucketset *bst, bucket *bt)
{
}
#ifdef BUCKETSET_TEST
int main(void) {
  log_init(LOG_ERR,LOG_STDOUT_TYPE,NULL);
  bucketset *bst = bucketset_create("test_set",3,16384,64);
  bucketset_print(bst);
  return 0;
}
#endif