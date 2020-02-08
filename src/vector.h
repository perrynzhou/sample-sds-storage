/*************************************************************************
  > File Name: vector.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Tue 31 Dec 2019 05:43:53 PM CST
 ************************************************************************/

#ifndef _VECTOR_H
#define _VECTOR_H
#include <stdint.h>
#include <pthread.h>
typedef void  (*vector_cb)(void *);
typedef struct vector_t
{
  uint64_t size;
  uint64_t cap;
  void **tables;
  pthread_mutex_t lock;
  vector_cb cb;
} vector;
vector *vector_create(uint64_t cap, vector_cb cb);
int vector_init(vector *vec, uint64_t cap, vector_cb cb);
int vector_push_back(vector *vec, void *data);
void *vector_at(vector *vec, uint64_t index);
int vector_insert(vector *vec,uint64_t index,void *data);
void vector_deinit(vector *vec);
void vector_destroy(vector *vec);
#endif
