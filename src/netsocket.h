/*************************************************************************
  > File Name: netsocket.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Sun 09 Feb 2020 10:33:47 AM CST
 ************************************************************************/

#ifndef _NETSOCKET_H
#define _NETSOCKET_H
#include "slice.h"
#include <ev.h>
typedef struct netsocket_t
{
  int port;
  slice addr;
  int backlog;
  struct ev_loop *loop;
  int sock;
} netsocket;
int netsocket_init(netsocket *ns, const char *addr, int port, int backlog);
void netsocket_start(netsocket *ns);
void netsocket_deinit(netsocket *ns);
#endif
