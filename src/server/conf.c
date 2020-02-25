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
    "backlog"
    "data_dir",
    "data_length"};
enum conf_keys_index
{
  CONF_ADDR = 0,
  CONF_PORT,
  CONF_BACKLOG,
  CONF_DATA_DIR,
  CONF_DATA_LENGTH
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
    cf->ctx = root;
    json_t *_port = json_object_get(root, conf_keys[CONF_PORT]);
    cf->port = (int)json_integer_value(_port);
    json_t *_addr = json_object_get(root, conf_keys[CONF_ADDR]);
    slice_init(&cf->addr, json_string_value(_addr));
    json_t *_backlog = json_object_get(root, conf_keys[CONF_BACKLOG]);
    cf->backlog = (int)json_integer_value(_backlog);

    json_t *_data_dir = json_object_get(root, conf_keys[CONF_DATA_DIR]);
    slice_init(&cf->data_dir, json_string_value(_data_dir));
    
    json_t *_data_length = json_object_get(root, conf_keys[CONF_DATA_LENGTH]);
    cf->data_length = (int)json_integer_value(_data_length);
    log_info_safe("conf_create success:\n%s\n", conf_dump(cf));
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
    slice_deinit(&cf->data_dir);
    cf = NULL;
  }
}   