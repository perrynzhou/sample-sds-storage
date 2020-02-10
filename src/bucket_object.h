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
#define BUCKET_OBJECT_UID_LEN  (32)
enum bucket_object_status {
   ONLINE=0,
   OFFLINE
};
typedef struct bucket_object_t {
  uint64_t cur_offset;
  uint8_t  status:1;
  uint8_t  uid[BUCKET_OBJECT_UID_LEN];
  uint32_t obj_name_len;
  uint64_t obj_hash;
  uint32_t bucket_index;
  uint64_t data_len;
}bucket_object;
bucket_object *bucket_object_create(uint64_t start_offset,uint8_t *obj_uid,uint32_t obj_name_len,uint64_t data_len);
void bucket_object_set_hash(bucket_object *obj,uint64_t hash);
void bucket_object_set_bucket_index(bucket_object *obj,uint32_t index);
int bucket_object_delete(bucket_object *obj);
void  bucket_object_destroy(bucket_object *obj);
#endif
