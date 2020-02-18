/*************************************************************************
  > File Name: request.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Wed 12 Feb 2020 01:51:32 PM CST
 ************************************************************************/

#ifndef _REQUEST_H
#define _REQUEST_H
#include <stdint.h>
#define BUCKET_OBJECT_UID_SZ (32)
#define BUCKET_BUFFER_LENGTH (255)
enum request_type
{
   REQ_PUT_OBJECT = 0,
   REQ_DEL_OBJECT,
   REQ_PUT_BUCKETSET,
};
typedef struct response_head_t
{
   uint8_t req_type;
   char name[BUCKET_BUFFER_LENGTH];
} response_head;
typedef struct response_ack_t
{
   response_head ack;
   int ret;
} response_ack;
typedef struct request_put_object_t
{
   char object_name[BUCKET_BUFFER_LENGTH];
   char set_name[BUCKET_BUFFER_LENGTH];
   uint8_t uid[BUCKET_OBJECT_UID_SZ];
   uint64_t data_length;
} request_put_object;
typedef struct request_put_bucketset_t
{
   char set_name[BUCKET_BUFFER_LENGTH];
   char bucket_name[BUCKET_BUFFER_LENGTH];
} request_put_bucketset;
int request_put_object_init(response_ack *ack, int sock, const char *path, const char *bucket_name);
int request_put_bucketset_init(response_ack *ack, int sock, const char *setname,const char *bucket_name);
#endif
