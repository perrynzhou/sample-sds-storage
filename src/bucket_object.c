/*************************************************************************
  > File Name: bucket_object.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 10:25:00 AM CST
 ************************************************************************/

#include "bucket_object.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
inline void bucket_object_set_hash(bucket_object *obj, uint64_t hash)
{
  if (obj != NULL)
  {
    obj->obj_hash = hash;
  }
}
inline void bucket_object_set_bucket_index(bucket_object *obj, uint32_t index)
{
  if (obj != NULL)
  {
    obj->bucket_index = index;
  }
}
bucket_object *bucket_object_create(uint64_t start_offset, uint8_t *obj_uid, uint32_t obj_name_len, uint64_t data_len)
{
  bucket_object *obj = (bucket_object *)calloc(1, sizeof(bucket_object));
  assert(obj != NULL);
  for (uint32_t i = 0; i < BUCKET_OBJECT_UID_LEN; i++)
  {
    obj->uid[i] = obj_uid[i];
  }
  obj->cur_offset = start_offset;
  obj->obj_name_len = obj_name_len;
  obj->obj_hash = 0;
  obj->data_len = data_len;
  obj->bucket_index = 0;
  obj->status = ONLINE;
  return obj;
}
int bucket_object_delete(bucket_object *obj)
{
  return 0;
}
void bucket_object_destroy(bucket_object *obj)
{
}