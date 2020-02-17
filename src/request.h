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
#define BUCKET_BUFFER_LENGTH   (255)
 enum  request_type{
   REQ_PUT_OBJECT=0,
   REQ_DEL_OBJECT,
   REQ_PUB_BUCKET,
};
typedef struct request_head_t {
     uint8_t req_type;
     uint32_t req_id;
}request_head;
typedef struct response_head_t {
    uint8_t req_type;
     uint32_t req_id;
     char    name[BUCKET_BUFFER_LENGTH];
}response_head;
typedef struct response_ack_t{
   response_head ack;
   int  ret;
}response_ack;
typedef struct request_put_object_t {
   request_head  head;
   char    name[BUCKET_BUFFER_LENGTH];
   uint8_t uid[BUCKET_OBJECT_UID_SZ];
   uint64_t data_length;
   char    data[];
}request_put_object;
typedef struct request_put_bucket_t {
     request_head  head;
       char    name[BUCKET_BUFFER_LENGTH];
}request_put_bucket;
int  request_put_object_init(response_ack *ack,int sock,const char *path);
int request_put_bucket_init(response_ack *ack,int sock,const char *name);
#endif
