/*************************************************************************
  > File Name: hash.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 12:18:27 PM CST
 ************************************************************************/

#ifndef _HASH_H
#define _HASH_H
#include <stdint.h>
uint64_t gfs_hashfn(const char *msg, int len);
uint32_t hash_jump_consistent(uint64_t key, int32_t num_buckets);
#endif
