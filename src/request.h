/*************************************************************************
  > File Name: request.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Wed 12 Feb 2020 01:51:32 PM CST
 ************************************************************************/

#ifndef _REQUEST_H
#define _REQUEST_H
#include <stdint.h>
#define BUCKET_OBJECT_UID_SZ  (32)
#define BUCKET_OBJECT_NAME_SZ   (255)
 enum  request_type{
   REQ_PUT_OBJECT=0,
   REQ_DEL_OBJECT,
};
typedef struct request_put_object_t {
   uint8_t req_type;
   uint32_t req_id;
   char    name[BUCKET_OBJECT_NAME_SZ];
   uint8_t uid[BUCKET_OBJECT_UID_SZ];
   uint64_t data_length;
   char    data[];
}request_put_object;
int request_put_object_init(int sock,const char *path);
#endif
