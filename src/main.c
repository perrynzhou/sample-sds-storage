/*************************************************************************
  > File Name: main.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Tue 18 Feb 2020 03:48:01 PM CST
 ************************************************************************/
#include "sds_store.h"
#include "request.h"
#include "netsocket.h"
#include "log.h"
#include <stdio.h>
int main(int argc, char *argv[])
{
  sds_store ss;
  netsocket net;
  sds_store_init(&ss, argv[1]);
  netsocket_init(&net, slice_value(&ss.cf->addr), ss.cf->port, ss.cf->backlog,&ss);
  netsocket_run(&net);
  netsocket_deinit(&net);
  sds_store_deinit(&ss);
  return 0;
}
