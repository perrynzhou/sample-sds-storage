/*************************************************************************
  > File Name: conf.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Sun 24 Nov 2019 04:14:50 PM CST
 ************************************************************************/

#ifndef _CONF_H
#define _CONF_H
#include "slice.h"
typedef struct conf_t
{
 int port;
 slice addr;
 slice data_dir;
 int  data_length;
 int backlog;
 int bucket_size;
 int set_hash_range;
 void *ctx;
}conf;
conf *conf_create(const char *path);
char *conf_dump(conf *cf);
void conf_destroy(conf *cf);
#endif
