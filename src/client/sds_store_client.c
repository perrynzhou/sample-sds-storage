/*************************************************************************
  > File Name: sds_store_client.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 21 Feb 2020 07:21:11 PM CST
 ************************************************************************/

#include "request.h"
#include "slice.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <getopt.h>
#define SDS_STORE_SERVER_ADDR "127.0.0.1"
#define SDS_STORE_SERVER_PORT (5566)
static const char *op_names[] = {
    "set",
    "object"};
static const char *op_types[] = {
    "create",
    "put",
    "get",
    "delete",
};
enum op_name_index
{
  SET_NAME_IDX,
  OBJECT_NAME_IDX
};
static int connect_server(const char *addr, int port)
{

  char *srv_addr_str = (addr == NULL) ? strdup(addr) : strdup(SDS_STORE_SERVER_ADDR);
  int srv_port = (port <= 1024) ? SDS_STORE_SERVER_PORT : port;

  struct sockaddr_in srv_addr;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_port = htons(srv_port);
  srv_addr.sin_addr.s_addr = inet_addr(srv_addr_str);
  bzero(&(srv_addr.sin_zero), sizeof(srv_addr.sin_zero));

  if (connect(sockfd, (struct sockaddr *)&srv_addr, sizeof(struct sockaddr)) == -1)
  {
    fprintf(stdout, "conneccted failed:%s\n", strerror(errno));
    goto out;
  }
  free(srv_addr_str);
  return sockfd;
out:
  if (srv_addr_str != NULL)
  {
    free(srv_addr_str);
  }
  if (sockfd != -1)
  {
    close(sockfd);
  }
  return -1;
}
int main(int argc, char *argv[])
{
  int ch;
  slice op_type;
  slice op_name;
  slice op_data;
  slice srv_addr;
  slice srv_port;
  const char *options = "rtdsp:";
  int option_index = 0;
  struct option long_options[] = {
      {"operation resource(set/object)", required_argument, NULL, 'r'},
      {"opetation type(put/get/delete)", required_argument, NULL, 't'},
      {"operation data(set:set_name,object:data_path)", required_argument, NULL, 'd'},
      {"sds server address", no_argument, NULL, 's'},
      {"sds server port", no_argument, NULL, 'p'},
      {0, 0, 0, 0}};

  while ((ch = getopt_long(argc, argv, options, long_options, &option_index)) != -1)
  {
    switch (ch)
    {
    case 't':
      if (slice_init(&op_type, optarg) != 0)
      {
        fprintf(stdout, "err:data is nil\n");
        exit(-1);
      }
      break;
    case 'r':
      if (slice_init(&op_name, optarg) != 0)
      {
        fprintf(stdout, "err:operation type  is nil\n");
        exit(-1);
      }
      break;
    case 'd':
      if (slice_init(&op_data, optarg) != 0)
      {
        fprintf(stdout, "err:data is nil\n");
        exit(-1);
      }
      break;
    case 's':
      slice_init(&srv_addr, optarg);
      break;
    case 'p':
      slice_init(&srv_port, optarg);
      break;
    }
  }
  int sockfd = connect_server(slice_value(&srv_addr), atoi(slice_value(&srv_port)));
  if (sockfd != -1)
  {
    if (strncmp(slice_value(&op_name), op_names[SET_NAME_IDX], slice_size(&op_name)) == 0)
    {
      response_ack ack;
      if(request_put_bucketset_init(&ack,sockfd,slice_value(&op_data))==-1) {
         log_err_safe("put bucketset failed");
         exit(1);
      }
      log_info_safe("put bucketset success,buckset name:%s,request type:%d",ack.ack.name,ack.ack.req_type);
    }
    else if (strncmp(slice_value(&op_name), op_names[OBJECT_NAME_IDX], slice_size(&op_name)) == 0)
    {
       
    }
  }
  slice_deinit(&op_type);
  slice_deinit(&op_name);
  slice_deinit(&op_data);
  slice_deinit(&srv_addr);
  slice_deinit(&srv_port);
  if (sockfd != -1)
  {
    close(sockfd);
  }
  return 0;
}