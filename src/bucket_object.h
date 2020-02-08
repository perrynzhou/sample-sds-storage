/*************************************************************************
  > File Name: bucket_object.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 10:24:55 AM CST
 ************************************************************************/

#ifndef _BUCKET_OBJECT_H
#define _BUCKET_OBJECT_H
#include "slice.h"
#include <stdint.h>
typedef struct bucket_object_t {
  uint64_t offset;
  uint64_t len;//sizeof(bucket_object)+data_len+file_name_len;
  uint32_t file_name_len;
  uint64_t bucket_index;
  uint64_t hash;
  uint64_t data_len;
}bucket_object;
bucket_object *bucket_object_create(const char *path,int dst_fd,uint64_t hash,uint64_t start_offset);
int bucket_object_delete(bucket_object *obj);
void  bucket_object_destroy(bucket_object *obj);
#endif
