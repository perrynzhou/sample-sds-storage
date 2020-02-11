/*************************************************************************
  > File Name: netsocket.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Sun 09 Feb 2020 10:33:52 AM CST
 ************************************************************************/

#include "netsocket.h"
#include "log.h"
#include "slice.h"
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
typedef struct ev_io_watcher_t
{
  struct ev_io watcher;
  void *ctx;
} ev_io_watcher;
typedef struct ev_timer_watcher_t
{
  ev_timer watcher;
  void *ctx;
} ev_timer_watcher;

static void timeout_cb(EV_P_ ev_timer *w, int revents);
static void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
static void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
static void write_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
static void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_sd;
  ev_io_watcher *eiw = (ev_io_watcher *)watcher;
  struct ev_io *w_client = (struct ev_io *)malloc(sizeof(struct ev_io));
  if (EV_ERROR & revents)
  {
    log_info("error event in accept\n");
    return;
  }

  client_sd = accept(watcher->fd, (struct sockaddr *)&client_addr, &client_len);
  if (client_sd < 0)
  {
    log_err("accept error\n");
    return;
  }
  log_info("someone connected.\n");

  ev_io_init(w_client, read_cb, client_sd, EV_READ);
  ev_io_start(loop, w_client);
}
static void timeout_cb(EV_P_ ev_timer *w, int revents)
{
  ev_timer_watcher *evw = (ev_timer_watcher *)w;
  int *value = (int *)evw->ctx;
  log_info_safe("##ev_timer ctx:%d", *value);
}
static void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  char buffer[1024];
  ssize_t read;
  if (EV_ERROR & revents)
  {
    printf("error event in read");
    return;
  }

  read = recv(watcher->fd, buffer, 1024, 0);
  if (read < 0)
  {
    log_err("read error,errno:%d\n", errno);
    return;
  }
  if (read == 0)
  {
    log_info("someone disconnected.errno:%d", errno);
    ev_io_stop(loop, watcher);
    free(watcher);
    return;
  }
  else
  {
    log_info("get the message:%s", buffer);
  }

  send(watcher->fd, buffer, read, 0);
  bzero(buffer, read);
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
    int sock;
    struct sockaddr_in addr;
    int addr_len = sizeof(addr);

    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
      log_fatal("socket error:%s", strerror(errno));
      return -1;
    }
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
      log_fatal("bind error:%s", strerror(errno));
    }
    if (listen(sock, backlog) < 0)
    {
      log_fatal("listen error:%s", strerror(errno));
      return -1;
    }
    ns->sock = sock;
    ns->loop = loop;
    int yes = 1;
    setsockopt(ns->sock, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(int));
    int n_recvbuf = 512 * 1024; //setting recv buffer size
    setsockopt(ns->sock, SOL_SOCKET, SO_RCVBUF, (const char *)&n_recvbuf, sizeof(int));
    int n_sendbuf = 512 * 1024; //setting send buffer size
    setsockopt(ns->sock, SOL_SOCKET, SO_SNDBUF, (const char *)&n_sendbuf, sizeof(int));

    log_info("init netsocekt_init success,addr:%s,port:%d,backlog:%d", slice_value(&ns->addr), port, backlog);
    return 0;
  }
  return -1;
}
void netsocket_start(netsocket *ns)
{
  log_info("netsocket start running");
  ev_io_watcher socket_watcher;
  socket_watcher.ctx = ns->ctx;
  //struct ev_io socket_watcher;
  ev_timer_watcher timeout_watcher;
  timeout_watcher.ctx = ns->ctx;

  ev_timer_init(&timeout_watcher.watcher, timeout_cb, 2, 1);
  ev_io_init(&socket_watcher.watcher, accept_cb, ns->sock, EV_READ);
  ev_io_start(ns->loop, &socket_watcher.watcher);
  ev_timer_start(ns->loop, &timeout_watcher.watcher);

  ev_run(ns->loop, 0);
}
void netsocket_deinit(netsocket *ns)
{
  if (ns != NULL && ns->sock != -1)
  {
    ev_break(ns->loop, EVBREAK_ALL);
    ev_loop_destroy(ns->loop);
    close(ns->sock);
  }
}
#ifdef NETSOCKET_TEST
int main(int argc, char *argv[])
{
  int port = atoi(argv[1]);
  netsocket ns;
  netsocket_init(&ns, NULL, port, 1024);
  netsocket_start(&ns);
  netsocket_deinit(&ns);
  return 0;
}
#endif