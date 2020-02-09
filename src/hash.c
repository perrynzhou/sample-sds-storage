/*************************************************************************
  > File Name: hash.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 07 Feb 2020 12:18:31 PM CST
 ************************************************************************/

#include "hash.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define DM_DELTA 0x9E3779B9
#define DM_FULLROUNDS 10 /* 32 is overkill, 16 is strong crypto */
#define DM_PARTROUNDS 6  /* 6 gets complete mixing */
static uint32_t __pad(int len)
{
  uint32_t pad = 0;

  pad = (uint32_t)len | ((uint32_t)len << 8);
  pad |= pad << 16;

  return pad;
}
static int dm_round(int rounds, uint32_t *array, uint32_t *h0, uint32_t *h1)
{
  uint32_t sum = 0;
  int n = 0;
  uint32_t b0 = 0;
  uint32_t b1 = 0;

  b0 = *h0;
  b1 = *h1;

  n = rounds;

  do
  {
    sum += DM_DELTA;
    b0 += ((b1 << 4) + array[0]) ^ (b1 + sum) ^ ((b1 >> 5) + array[1]);
    b1 += ((b0 << 4) + array[2]) ^ (b0 + sum) ^ ((b0 >> 5) + array[3]);
  } while (--n);

  *h0 += b0;
  *h1 += b1;

  return 0;
}
uint64_t gfs_hashfn(const char *msg, int len)
{
  uint32_t h0 = 0x9464a485;
  uint32_t h1 = 0x542e1a94;
  uint32_t array[4];
  uint32_t pad = 0;
  int i = 0;
  int j = 0;
  int full_quads = 0;
  int full_words = 0;
  int full_bytes = 0;
  uint32_t *intmsg = NULL;
  int word = 0;

  intmsg = (uint32_t *)msg;
  pad = __pad(len);

  full_bytes = len;
  full_words = len / 4;
  full_quads = len / 16;

  for (i = 0; i < full_quads; i++)
  {
    for (j = 0; j < 4; j++)
    {
      word = *intmsg;
      array[j] = word;
      intmsg++;
      full_words--;
      full_bytes -= 4;
    }
    dm_round(DM_PARTROUNDS, &array[0], &h0, &h1);
  }

  for (j = 0; j < 4; j++)
  {
    if (full_words)
    {
      word = *intmsg;
      array[j] = word;
      intmsg++;
      full_words--;
      full_bytes -= 4;
    }
    else
    {
      array[j] = pad;
      while (full_bytes)
      {
        array[j] <<= 8;
        array[j] |= msg[len - full_bytes];
        full_bytes--;
      }
    }
  }
  dm_round(DM_FULLROUNDS, &array[0], &h0, &h1);

  return (uint64_t)(h0 ^ h1);
}
uint32_t hash_jump_consistent(uint64_t key, int32_t num_buckets)
{

  int64_t b = -1, j = 0;
  uint32_t value = 0;
  while (j < num_buckets)
  {
    b = j;
    key = key * 2862933555777941757ULL + 1;
    j = (b + 1) * ((double)(1LL << 31) / (double)((key >> 33) + 1));
  }
  value = (b < 0) ? (~b + 1) : b;
  return value;
}
#ifdef HASH_TEST
int main(int argc, const char *argv[])
{
  uint32_t n = 163840;
  int *sum = calloc(n, sizeof(uint32_t));
  for (uint32_t i = 0; i < n; i++)
  {
    char buf[1024] = {'\0'};
    snprintf((char *)&buf, 1024, "%d.txt", rand());
    uint32_t hash = gfs_hashfn((char *)&buf, strlen((char *)&buf));
    uint32_t h = hash_jump_consistent(hash, 4);
    fprintf(stdout, "hash:%ld,sum[%d] =%d\n", h, h, sum[h]);
      sum[h] = sum[h] + 1;
  }

  for (uint32_t i = 0; i < n; i++)
  {
    if (sum[i] > 3)
    {
      fprintf(stdout, "sum[%d]=%d\n",i, sum[i]);
    }
  }
  free(sum);
}
#endif