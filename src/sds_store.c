/*************************************************************************
  > File Name: sds_store.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Mon 10 Feb 2020 02:02:39 PM CST
 ************************************************************************/

#include "sds_store.h"
#include "hash.h"
#include "list.h"
#include "netsocket.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#define SDS_STORE_MAX_SET (8192)
int sds_store_init(sds_store *ss, const char *conf_path)
{
  if (ss != NULL && conf_path != NULL)
  {
    conf *cf = conf_create(conf_path);
    assert(cf != NULL);
    vector_init(&ss->sets, SDS_STORE_MAX_SET, NULL);
    pthread_mutex_init(&ss->lock, NULL);
    ss->cf = cf;
    return 0;
  }
  return -1;
}
int sds_store_add(sds_store *ss, bucketset *bst)
{
  uint32_t sz = ss->sets.size;
  uint64_t hash = gfs_hashfn(slice_value(&bst->set_name), slice_size(&bst->set_name));
  uint32_t index = hash_jump_consistent(hash, sz);
  list *lt = (list *)vector_at(&ss->sets, index);
  pthread_mutex_lock(&ss->lock);
  if (lt == NULL)
  {
    lt = list_create();
    vector_insert(&ss->sets, index, lt);
  }
  list_node *n = list_node_create(bst);
  assert(n != NULL);
  list_add(lt, n);
  pthread_mutex_unlock(&ss->lock);
  return 0;
}
void sds_store_deinit(sds_store *ss)
{
  if (ss != NULL)
  {

    vector_destroy(&ss->sets);
    conf_destroy(ss->cf);
    //netsocket_deinit(&ss->net);
  }
}
/*
int sds_store_run(sds_store *ss)
{
  if (ss != NULL)
  {
    
    ss->net.ctx = ss;
    //ns.ctx = ss;
    netsocket_init(&ss->net, slice_value(&ss->cf->addr), ss->cf->port, ss->cf->backlog);
    netsocket_start(&ss->net);
  }
}
*/
int main(int argc, char *argv[])
{
  char *path = argv[1];
  sds_store ss;

  sds_store_init(&ss, path);
  sds_store_deinit(&ss);
  return 0;
}