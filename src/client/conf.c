/*************************************************************************
  > File Name: conf.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Sun 24 Nov 2019 04:14:58 PM CST
 ************************************************************************/
#include "conf.h"
#include "slice.h"
#include "log.h"
#include <stdio.h>
#include <stdbool.h>
#include <jansson.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#define CONF_BUFFER_MAX (1024 * 1024)

const static char *conf_keys[] = {
    //gluserfs
    "addr",
    "port",
    "bucketset"};
enum conf_keys_index
{
  CONF_ADDR = 0,
  CONF_PORT,
  CONF_BUCKETSET
};
conf *conf_create(const char *path)
{
  json_error_t err;
  int fd = open(path, O_RDONLY);
  char buf[CONF_BUFFER_MAX] = {'\0'};
  if (fd == -1 || read(fd, &buf, CONF_BUFFER_MAX) < 0)
  {
    goto ERR;
  }
  conf *cf = (conf *)calloc(1, sizeof(*cf));
  if (cf != NULL)
  {
    json_t *root = json_loads((char *)&buf, 0, &err);
    if (root == NULL)
    {
      goto ERR;
    }
    json_t *_port = json_object_get(root, conf_keys[CONF_PORT]);
    cf->port = (int)json_integer_value(_port);
    json_t *_addr = json_object_get(root, conf_keys[CONF_ADDR]);
    slice_init(&cf->addr, json_string_value(_addr));
    log_info_safe("conf_create success:\n%s\n", conf_dump(cf));
    cf->ctx = root;
    return cf;
  }
ERR:
  log_err_safe("conf_create err:%s", path, strerror(errno));
  if (fd != -1)
  {
    close(fd);
  }
  if(cf !=NULL) {
    free(cf);
  }
  return NULL;
}
char *conf_dump(conf *cf)
{
  json_t *root = (json_t *)cf->ctx;
  return json_dumps(root, 0);
}
void conf_destroy(conf *cf)
{
  if (cf != NULL)
  {
    slice_deinit(&cf->addr);
    free(cf);
    cf = NULL;
  }
}   