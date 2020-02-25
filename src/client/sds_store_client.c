/*************************************************************************
  > File Name: sds_store_client.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 21 Feb 2020 07:21:11 PM CST
 ************************************************************************/

#include "request.h"
#include "slice.h"
#include "conf.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
enum op_name_index
{
  BUCKSET_NAME_IDX,
  OBJECT_NAME_IDX
};
static const char *op_resource_names[] = {
    "buckset",
    "object"};

static const char *op_set_cmds[] = {
    "create",
    "list",
    "delete"};
static const char *op_objects_cmds[] = {
    "put",
    "get",
    "delete"};
static const char *default_confs_path[] = {
    "/etc/sds/client.json",
    "./client.json",
};
static int connect_server(const char *addr, int port)
{

  struct sockaddr_in srv_addr;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_port = htons(port);
  srv_addr.sin_addr.s_addr = inet_addr(addr);
  bzero(&(srv_addr.sin_zero), sizeof(srv_addr.sin_zero));

  if (connect(sockfd, (struct sockaddr *)&srv_addr, sizeof(struct sockaddr)) == -1)
  {
    fprintf(stdout, "conneccted failed:%s\n", strerror(errno));
    goto out;
  }
  return sockfd;
out:
  if (sockfd != -1)
  {
    close(sockfd);
  }
  return -1;
}
static bool check_conf(char **path)
{
  size_t count = sizeof(default_confs_path) / sizeof(default_confs_path[0]);
  struct stat st;
  for (int i = 0; i < count; i++)
  {
    if (stat(default_confs_path[i], &st) == 0)
    {
      *path = strdup(default_confs_path[i]);
      return true;
    }
  }
  return false;
}
/*

sds_cli -r bucketset -t create -d test_set
sds_cli -r object  -t put -d {buckset_name}:/test/datafile

*/
int main(int argc, char *argv[])
{
  int ch;
  slice resource_name;
  slice op_type;
  slice op_data;
  const char *options = "rtd:";
  int option_index = 0;
  char *conf_path = NULL;
  if (!check_conf(&conf_path))
  {
    fprintf(stdout, "client conf is nil");
    exit(0);
  }
  conf *cf = conf_create(conf_path);
  int sockfd = connect_server(slice_value(&cf->addr), cf->port);
  if (sockfd == -1)
  {
    goto out;
  }

  struct option long_options[] = {
      {"operation resource(set/object)", required_argument, NULL, 'r'},
      {"opetation type(put/get/delete)", required_argument, NULL, 't'},
      {"operation data(if bucketset,that is {buckset-name};if object,that is {setname:/datafile})", required_argument, NULL, 'd'},
      {0, 0, 0, 0}};

  while ((ch = getopt_long(argc, argv, options, long_options, &option_index)) != -1)
  {
    switch (ch)
    {
    case 't':
      if (slice_init(&op_type, optarg) != 0)
      {
        fprintf(stdout, "operation type is nil\n");
        exit(-1);
      }
      break;
    case 'r':
      if (slice_init(&resource_name, optarg) != 0)
      {
        fprintf(stdout, "resource support:bucketset/object\n");
        exit(-1);
      }
      break;
    case 'd':
      if (slice_init(&op_data, optarg) != 0)
      {
        fprintf(stdout, "inout   data is nil\n");
        exit(-1);
      }
      break;
    }
  }
  if (sockfd != -1)
  {
    if (strncmp(slice_value(&resource_name), op_resource_names[BUCKSET_NAME_IDX], slice_size(&resource_name)) == 0)
    {
      response_ack ack;
      if (request_put_bucketset_init(&ack, sockfd, slice_value(&op_data)) == -1)
      {
        log_err_safe("put bucketset failed");
        exit(1);
      }
      log_info_safe("put bucketset success,buckset name:%s,request type:%d", ack.ack.name, ack.ack.req_type);
    }
    else if (strncmp(slice_value(&resource_name), op_resource_names[OBJECT_NAME_IDX], slice_size(&resource_name)) == 0)
    {
    }
  }
out:
  slice_deinit(&op_type);
  slice_deinit(&resource_name);
  slice_deinit(&op_data);
  conf_destroy(cf);
  if (sockfd != -1)
  {
    close(sockfd);
  }
  return 0;
}