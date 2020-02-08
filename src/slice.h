/*************************************************************************
  > File Name: slice.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Mon 28 Oct 2019 02:22:06 PM CST
 ************************************************************************/

#ifndef _SLICE_H
#define _SLICE_H
#include <stdio.h>
#define SLICE_INTERNAL_SZ (16)
typedef struct slice_t
{
    union {
        char buf[SLICE_INTERNAL_SZ];
        char *ptr;
    } data;
    size_t sz;
} slice;
slice *slice_create(const char *value);
int slice_init(slice *se, const char *value);
slice *slice_create_with_fmt(const char *fmt, ...);
int slice_strncpy(slice *se, const char *str, size_t n);
char *slice_value(slice *se);
size_t slice_size(slice *se);
int slice_repace_value(slice *se, const char *value);
int slice_copy(slice *dest, slice *src);
void slice_print(slice *se);
void slice_deinit(slice *se);
void slice_destroy(slice *se);
#endif
