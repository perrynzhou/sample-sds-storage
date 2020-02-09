/*************************************************************************
  > File Name: netsocket.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Sun 09 Feb 2020 10:33:52 AM CST
 ************************************************************************/

#include "netsocket.h"
#include "log.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <netinet/in.h>
#include <strings.h>
#define NETSOCKET_DEFAULT_BACKLOG (1024)
#define NETSOCKET_DEFAULT_PORT 8765
#define NETSOCKET_DEFAULT_ADDR "127.0.0.1"
static void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
}
static void timeout_cb(EV_P_ ev_timer *w, int revents)
{
}
static void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
}
static void write_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
}
int netsocket_init(netsocket *ns, const char *saddr, int port, int backlog)
{
  if (ns != NULL)
  {
    port = (port <= 1024) ? NETSOCKET_DEFAULT_PORT : port;
    if (saddr != NULL)
    {
      slice_init(&ns->addr, saddr);
    }
    else
    {
      slice_init(&ns->addr, NETSOCKET_DEFAULT_ADDR);
    }
    backlog = (backlog < NETSOCKET_DEFAULT_BACKLOG) ? NETSOCKET_DEFAULT_BACKLOG : backlog;
    struct ev_loop *loop = ev_default_loop(0);
    int sd;
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);
    struct ev_io socket_watcher;
    ev_timer timeout_watcher;

    if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
      log_fatal("socket error:%s", strerror(errno));
      return -1;
    }
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
      log_fatal("bind error:%s", strerror(errno));
    }
    if (listen(sd, backlog) < 0)
    {
      log_fatal("listen error:%s", strerror(errno));
      return -1;
    }

    ev_io_init(&socket_watcher, accept_cb, sd, EV_READ);
    ev_io_start(loop, &socket_watcher);

    ev_timer_init(&timeout_watcher, timeout_cb, 2, 1);
    ev_timer_start(loop, &timeout_watcher);
    log_info("init netsocekt_init success,addr:%s,port:%d,backlog:%d", slice_value(&ns->addr), port, backlog);
    return 0;
  }
  return -1;
}
void netsocket_start(netsocket *ns)
{
  log_info("netsocket start running");
  for (;;)
  {
    log_info("listen loop begin");
    ev_loop(ns->loop, 0);
  }
}
void netsocket_deinit(netsocket *ns)
{
}