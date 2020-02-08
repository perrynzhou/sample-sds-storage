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
bucket_object *bucket_object_create(const char *path, int dst_fd, uint64_t hash, uint64_t start_offset)
{
  bucket_object *obj = NULL;
  if (path != NULL && dst_fd != -1)
  {
    obj = (bucket_object *)calloc(1, sizeof(bucket_object));
    slice object_name;
    parse_filename_from_path(&object_name, path);
    obj->hash = hash;
    char *buffer = calloc(1024 * 1024, sizeof(char));
    assert(buffer != NULL);
    int fd = open(path, O_RDONLY);
    assert(fd != -1);
    ssize_t obj_data_len = write_file(fd, dst_fd, buffer, 1024 * 1024);
    obj->cur_offset = start_offset;
    obj->obj_name_len = slice_size(&object_name);
    obj->next_offset = sizeof(bucket_object)+BUCKET_OBJECT_UID_LEN+obj->obj_name_len+obj->obj_uid_len+obj_data_len;
    slice_deinit(&object_name);
    if(buffer!=NULL) {
      free(buffer);
    }
    return  obj;
  }
  return NULL;
}
int bucket_object_delete(bucket_object *obj)
{
  return 0;
}
void bucket_object_destroy(bucket_object *obj)
{
}