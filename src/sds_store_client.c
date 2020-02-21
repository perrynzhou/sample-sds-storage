/*************************************************************************
  > File Name: sds_store_client.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Fri 21 Feb 2020 07:21:11 PM CST
 ************************************************************************/

#include "request.h"
#include "slice.h"
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
#define SDS_STORE_SERVER_ADDR "127.0.0.1"
#define SDS_STORE_SERVER_PORT (5566)
static const char *op_names[] = {
    "bucket",
    "object"};
static const char *op_types[] = {
    "create",
    "put",
    "get",
    "delete",
};
static int connect_server()
{

  struct sockaddr_in srv_addr;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  srv_addr.sin_family = AF_INET;
  srv_addr.sin_port = htons(8000);
  srv_addr.sin_addr.s_addr = inet_addr(SDS_STORE_SERVER_ADDR);
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
int main(int argc, char *argv[])
{
  int ch;
  slice type;
  slice op;
  slice data;
  while ((ch = getopt(argc, argv, "tod:")) != -1)
  {
    switch (ch)
    {
    case 't':
      if (slice_init(&type, optarg) != 0)
      {
        fprintf(stdout, "err:data is nil\n");
        exit(-1);
      }
      break;
    case 'o':
      if (slice_init(&op, optarg) != 0)
      {
        fprintf(stdout, "err:operation type  is nil\n");
        exit(-1);
      }
      break;
    case 'd':
      if (slice_init(&data, optarg) != 0)
      {
        fprintf(stdout, "err:data is nil\n");
        exit(-1);
      }
      break;
    }
  }
  int op_type = -1;
  if (strncmp(slice_value(&op), "bucket", slice_size(&op)) == 0)
  {
    op_type = 0;
  }
  else if (strncmp(slice_value(&op), "object", slice_size(&op)) == 0)
  {
    op_type = 1;
  }

  int sockfd = connect_server();
  if (sockfd != -1)
  {
  }
out:
  slice_deinit(&type);
  slice_deinit(&op);
  slice_deinit(&data);

  if (sockfd != -1)
  {
    close(sockfd);
  }
  return 0;
}