/*************************************************************************
  > File Name: netsocket.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Sun 09 Feb 2020 10:33:52 AM CST
 ************************************************************************/

#include "netsocket.h"
#include "log.h"
#include "request.h"
#include "hash.h"
#include "sds_store.h"
#include "utils.h"
#include "slice.h"
#include "vector.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <netinet/in.h>
#include <strings.h>
#include <assert.h>
#include <pthread.h>
#define NETSOCKET_DEFAULT_BACKLOG (1024)
#define NETSOCKET_DEFAULT_PORT 8765
#define NETSOCKET_DEFAULT_ADDR "127.0.0.1"
#define NETSOCKET_DEFAULT_BUFFER_LENGTH (4 * 1024 * 1024)
typedef void (*ev_read_callback)(struct ev_loop *loop, struct ev_io *watcher, int revents);
typedef void (*ev_write_callback)(struct ev_loop *loop, struct ev_io *watcher, int revents);
typedef void (*ev_timeout_callback)(EV_P_ ev_timer *w, int revents);
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
typedef struct ev_thread_t
{
  int fd;
  pthread_t tid;
  ev_io_watcher *ew;
  struct ev_loop *loop;
} ev_thread;
static void timeout_cb(EV_P_ ev_timer *w, int revents);
static void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
static void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
static void write_cb(struct ev_loop *loop, struct ev_io *watcher, int revents);
static ev_thread *ev_thread_create(int fd,
                                   ev_io_watcher *ew,
                                   struct ev_loop *loop)
{
  ev_thread *ev_thd = (ev_thread *)calloc(1, sizeof(ev_thread));
  assert(ev_thd != NULL);
  ev_thd->ew = ew;
  ev_thd->fd = fd;
  ev_thd->loop = loop;
  return ev_thd;
}

static void ev_thread_destroy(void *ev_thd)
{
  ev_thread **ev_thd_ptr = (ev_thread **)ev_thd;
  if (ev_thd_ptr != NULL)
  {
    free(*ev_thd_ptr);
    *ev_thd_ptr = NULL;
  }
}
static void *netsocket_do_request(void *arg)
{
  ev_thread **ev_thd_ptr = (ev_thread **)arg;
  if (ev_thd_ptr != NULL && *ev_thd_ptr != NULL)
  {
    ev_thread *ev_thd = (*ev_thd_ptr);
    struct ev_io *w_client = (struct ev_io *)malloc(sizeof(struct ev_io));
    ev_io_init(w_client, read_cb, ev_thd->fd, EV_READ);
    ev_io_start(ev_thd->loop, w_client);
    ev_thread_destroy(arg);
    free(w_client);
    pthread_detach(ev_thd->tid);
  }
}
static void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_fd;
  ev_io_watcher *eiw = (ev_io_watcher *)watcher;
  if (EV_ERROR & revents)
  {
    log_info("error event in accept\n");
    return;
  }

  client_fd = accept(watcher->fd, (struct sockaddr *)&client_addr, &client_len);
  if (client_fd < 0)
  {
    log_err("accept error\n");
    return;
  }
  char client_host[64] = {'\0'};
  fetch_sock_addr_info(client_fd, (char *)&client_host);
  log_info("%s connected", (char *)&client_host);
  ev_thread *e_thd = ev_thread_create(client_fd, eiw, loop);
  if (e_thd != NULL)
  {
    pthread_create(&e_thd->tid, NULL, (void *)&netsocket_do_request, (void *)&e_thd);
  }
  //ev_io_init(w_client, read_cb, client_sd, EV_READ);
  //ev_io_start(loop, w_client);
}
static void timeout_cb(EV_P_ ev_timer *w, int revents)
{
  ev_timer_watcher *evw = (ev_timer_watcher *)w;
  sds_store *ss = (sds_store *)evw->ctx;
  if(ss!=NULL && ss->cf!=NULL) {
       log_info_safe("----------------------conf info------------\n%s",conf_dump(ss->cf));
  }
}
static void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
  ev_io_watcher *eiw=(ev_io_watcher *)watcher;
  sds_store *st=(sds_store *)eiw->ctx;
  char *buffer = (char *)calloc(NETSOCKET_DEFAULT_BUFFER_LENGTH, sizeof(char));
  request_put_bucketset req_bucketset;
  request_put_object req_object;
  bucketset *sets=NULL;
  bucket   *bkt=NULL;
  assert(buffer != NULL);
  ssize_t read;
  if (EV_ERROR & revents)
  {
    printf("error event in read");
    return;
  }
  uint8_t req_type = 0;
  read = recv(eiw->watcher.fd, &req_type, sizeof(uint8_t), 0);
  if (read > 0)
  {
    log_err("read error,errno:%d\n", errno);
    return;
  }
  size_t len = 0;
    response_ack resp;
    bucketset *bst=NULL;
    uint64_t hash=0;
  switch (req_type)
  {
  case REQ_PUT_OBJECT:
    read_n(eiw->watcher.fd, &req_object, sizeof(req_object));
    len = strlen((char *)&req_object.uid);
     hash=gfs_hashfn((char *)&req_object.uid,len);
    for(int i=0;i<vector_size(&st->sets);i++) {
       bst =(bucketset *)vector_at(&st->sets,i);
       if(strncmp(slice_value(&bst->set_name),(char *)&req_object.set_name,slice_size(&bst->set_name))==0) {
          sets = bst;
          break;
       }
    }
    if(sets==NULL) {
      resp.ret = -1;
    }else{
          bucket *bt = bucketset_search_bucket(sets,hash);
          bucket_object obj;
          bucket_object_init(&obj,obj.bucket_index,bt->current_offset,(uint8_t *)req_object.uid,strlen(req_object.object_name),req_object.data_length);
          bucket_put(bt,&obj,eiw->watcher.fd);
    }
    break;
  case REQ_PUT_BUCKETSET:
    read_n(eiw->watcher.fd, &req_bucketset, sizeof(req_bucketset));
    for(int i=0;i<vector_size(&st->sets);i++) {
       bucketset *bst =(bucketset *)vector_at(&st->sets,i);
       if(strncmp(slice_value(&bst->set_name),(char *)&req_bucketset.set_name,slice_size(&bst->set_name))==0) {
          sets = bst;
          break;
       }
    }
    if(sets==NULL || vector_size(&st->sets)==0) {
      bucketset *bst = bucketset_create((char *)&req_bucketset.set_name,slice_value(&st->cf->data_dir),st->cf->bucket_size,st->cf->set_hash_range,st->cf->data_length);
      vector_push_back(&st->sets,bst);
    }
      resp.ret=0;
    break;
  default:
    break;
  }

  resp.ack.req_type = req_type;
  strncpy((char *)&resp.ack.name,(char *)&req_bucketset.set_name,strlen((char *)&req_bucketset.set_name));
  send(eiw->watcher.fd, &resp,sizeof(resp), 0);
  ev_io_stop(loop, watcher);

}
static void write_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
}
int netsocket_init(netsocket *ns, const char *saddr, int port, int backlog,void *ctx)
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
    ns->ctx = ctx;
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
void netsocket_run(netsocket *ns)
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