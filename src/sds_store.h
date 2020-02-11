/*************************************************************************
  > File Name: sds_store.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Mon 10 Feb 2020 02:02:10 PM CST
 ************************************************************************/

#ifndef _SDS_STORE_H
#define _SDS_STORE_H
#include "vector.h"
#include "conf.h"
#include "bucketset.h"
#include "netsocket.h"
#include <pthread.h>
typedef struct sds_store_t {
  vector sets;//each index save list,list save bucetsets
  conf *cf;
  netsocket net;
  pthread_mutex_t lock;
}sds_store;
int sds_store_init(sds_store *ss,const char *conf_path);
int sds_store_add(sds_store *ss,bucketset *bst);
vector *sds_store_list(sds_store *ss);
int sds_store_run(sds_store *ss);
void sds_store_deinit(sds_store *ss);
#endif
