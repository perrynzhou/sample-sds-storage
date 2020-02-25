/*************************************************************************
  > File Name: vector.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Tue 31 Dec 2019 05:44:02 PM CST
 ************************************************************************/

#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
vector *vector_create(uint64_t cap, vector_cb cb)
{
  vector *vec = (vector *)calloc(1, sizeof(vector));
  assert(vec != NULL);
  if (vector_init(vec, cap, cb) != 0)
  {
    free(vec->tables);
    return NULL;
  }
  return vec;
}
size_t  vector_size(vector *vec){
    return vec->size;
}
int vector_init(vector *vec, uint64_t cap, vector_cb cb)
{
  if (vec != NULL)
  {
    vec->cap = cap;
    vec->size = 0;
    pthread_mutex_init(&vec->lock, NULL);
    vec->tables = (void **)calloc(cap, sizeof(void *));
    vec->cb = cb;
    assert(vec->tables);
    return 0;
  }
  return -1;
}
static int vector_expand(vector *vec)
{
  void **tables = (void **)calloc(vec->cap * 2, sizeof(void *));
  if (tables != NULL)
  {
    for (uint64_t i = 0; i < vec->size; i++)
    {
      tables[i] = vec->tables[i];
    }
    free(vec->tables);
    vec->tables = tables;
    vec->cap = vec->cap * 2;
#ifdef VECTOR_TEST
    fprintf(stdout, "### expand size:%lld,cap:%lld###\n", vec->size, vec->cap);
#endif
    return 0;
  }
  return -1;
}
int vector_insert(vector *vec, uint64_t index, void *data)
{
  int ret = -1;
  if (data != NULL)
  {
    pthread_mutex_lock(&vec->lock);
    if (vec->size == vec->cap)
    {
      if (vector_expand(vec) != 0)
      {
        pthread_mutex_unlock(&vec->lock);
        return ret;
      }
    }
    vec->tables[index] = data;
    pthread_mutex_unlock(&vec->lock);
    ret = 0;
  }
  return ret;
}
int vector_push_back(vector *vec, void *data)
{
  if (data != NULL)
  {
    pthread_mutex_lock(&vec->lock);
    if (vec->size == vec->cap)
    {
      if (vector_expand(vec) != 0)
      {
        pthread_mutex_unlock(&vec->lock);
        return -1;
      }
    }
    vec->tables[vec->size++] = data;
    pthread_mutex_unlock(&vec->lock);
    return 0;
  }
  return -1;
}
inline void *vector_at(vector *vec, uint64_t index)
{
  if (index < vec->size)
  {
    return vec->tables[index];
  }
  return NULL;
}
void vector_deinit(vector *vec)
{
  if (vec != NULL)
  {
    pthread_mutex_lock(&vec->lock);
    if (vec->cb != NULL)
    {
      for (uint64_t i = 0; i < vec->size; i++)
      {
        vec->cb(vec->tables[i]);
      }
    }
    free(vec->tables);
    vec->tables = NULL;
    vec->size = 0;
    vec->cap = 0;
    pthread_mutex_unlock(&vec->lock);
  }
}
void vector_destroy(vector *vec)
{
  if (vec != NULL)
  {
    vector_deinit(vec);
    free(vec);
    vec = NULL;
  }
}

#ifdef VECTOR_TEST
static inline int vector_cb_free(void *ptr)
{
  free(ptr);
  return 0;
}
#include <stdlib.h>
int main(int argc, char *argv[])
{
  int cap = atoi(argv[1]);
  vector *vec = vector_create(cap, (vector_cb)vector_cb_free);
  size_t sz = 1024 * 1024 * 32;
  for (uint64_t i = 0; i < sz; i++)
  {
    int *c = calloc(1, sizeof(int));
    *c = rand() % 1024;
    vector_push_back(vec, c);
  }
  vector_destroy(vec);
  return 0;
}
#endif