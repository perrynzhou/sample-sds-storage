/*************************************************************************
  > File Name: slice.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Mon 28 Oct 2019 02:22:11 PM CST
 ************************************************************************/
#include "slice.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

int slice_repace_value(slice *se, const char *value)
{
    slice_deinit(se);
    return slice_init(se, value);
}
int slice_strncpy(slice *se, const char *str, size_t n)
{
    if (se !=NULL && str !=NULL && n > 0)
    {
        size_t len = strlen(str);
        se->sz = (n > len) ? len : n;
        bool flag = (len < SLICE_INTERNAL_SZ) ? true : false;
        if (flag)
        {
            memcpy(&se->data.buf, str, n);
            se->data.buf[se->sz] = '\0';
            return 0;
        }
        se->data.ptr = (char *)calloc(1, se->sz + 1);
        memcpy(&se->data.buf, str, se->sz);
        return 0;
    }
}

int slice_init(slice *se, const char *value)
{
    if (se != NULL)
    {
        if (value != NULL)
        {
            // SLICE_INTERNAL_SZ
            size_t value_sz = strlen(value);
            bool flag = (value_sz < SLICE_INTERNAL_SZ) ? true : false;
            se->sz = value_sz;
            if (flag)
            {
                memcpy(&se->data.buf, value, value_sz);
                se->data.buf[value_sz] = '\0';
                return 0;
            }
            se->data.ptr = strdup(value);
        }else{
            se->sz = 0;
            se->data.ptr = NULL;
        }
        return 0;
    }
    return -1;
}
slice *slice_create_with_fmt(const char *fmt, ...)
{
    va_list arg_origin;
    va_list arg_save;
    int size = 0;

    slice *se = (slice *)calloc(1, sizeof(slice));
    va_start(arg_origin, fmt);
    va_copy(arg_save, arg_origin);
    va_end(arg_origin);
    size = vsnprintf(NULL, 0, fmt, arg_origin);

    if (size < SLICE_INTERNAL_SZ)
    {
        vsnprintf((char *)&se->data.buf, size+1, fmt, arg_save);
        se->data.buf[size] = '\0';
    }
    else
    {
        se->data.ptr = (char *)calloc(sizeof(char), size + 1);
        vsnprintf(se->data.ptr, size + 1, fmt, arg_save);
        se->data.ptr[size] = '\0';
    }
    se->sz = size;
    va_end(arg_save);
    return se;
}
inline slice *slice_create(const char *value)
{
    slice *se = NULL;
    if (value != NULL)
    {
        se = (slice *)calloc(1, sizeof(slice));
        if (slice_init(se, value) == 0)
        {
            return se;
        }
        if (se != NULL)
        {
            free(se);
            se = NULL;
        }
    }
    return NULL;
    
}
inline size_t slice_size(slice *se)
{
    if (se != NULL)
    {
        return se->sz;
    }
    return 0;
}
char *slice_value(slice *se)
{
    char *value = NULL;
    if (se != NULL)
    {
        if (se->sz>0 && se->sz < SLICE_INTERNAL_SZ)
        {
            value = (char *)&se->data.buf;
        }
        else if(se->sz>0 &&se->sz >= SLICE_INTERNAL_SZ )
        {
            value = se->data.ptr;
        }
    }
    return value;
}
void slice_print(slice *se)
{
    if (se != NULL)
    {
        char *value = slice_value(se);
        if (value != NULL)
        {
            fprintf(stdout, "value:%s,value_len:%d\n", value,se->sz);
        }
    }
}
inline void slice_destroy(slice *se)
{
    if (se != NULL)
    {
        slice_deinit(se);
        free(se);
        se = NULL;
    }
}
int slice_copy(slice *dest, slice *src)
{
    if (dest != NULL && src != NULL)
    {
        char *value = slice_value(src);
        return slice_init(dest, value);
    }
    return -1;
}
inline void slice_deinit(slice *se)
{
    if (se != NULL && se->sz > 0)
    {
        if (se->sz >= SLICE_INTERNAL_SZ)
        {
            free(se->data.ptr);
            se->sz = 0;
        }
    }
}
#ifdef TEST
int main(void)
{
    slice *se = slice_create_with_fmt("%s fuck you!", "hello");
    return 0;
}
#endif