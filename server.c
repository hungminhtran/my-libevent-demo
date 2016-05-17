#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <arpa/inet.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "lmdb.h"
#include "lmdbInterface.h"

#define PORT_USE 8888
#define DB_ENV "./demoDB"
#define DB_NAME "what"

static void echo_read_cb(struct bufferevent *bev, void *ctx)
{

    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *output = bufferevent_get_output(bev);
    
    size_t len = evbuffer_get_length(input);
    char *keyVal;
    keyVal = malloc(MAX_KEY_ALLOCATE_SIZE);
    memset(keyVal, 0, MAX_KEY_ALLOCATE_SIZE);
    ev_ssize_t tlen = evbuffer_copyout(input, keyVal, len);
    if (tlen < 0)
        fprintf(stderr, "error when copy input dat");

    // fprintf(stderr, "input data %c", keyVal[0]);

    char *data;
    data = malloc(MAX_DATA_ALLOCATE_SIZE);
    getDataFromDB(DB_ENV, DB_NAME, keyVal[0], &data);
    fprintf(stderr, "data gotten:%s", data);
    evbuffer_add(output, data, MAX_DATA_ALLOCATE_SIZE);
    free(keyVal);
    free(data);
    // evbuffer_add_buffer(output, input);
}

static void echo_write_cb(struct bufferevent *bev, void *ctx)
{
    fprintf(stderr, "call write cb \n");
}

static void echo_event_cb(struct bufferevent *bev, short events, void *ctx)
{
    if (events & BEV_EVENT_ERROR ) {
        fprintf(stderr, "Close connection because of error\n");
        bufferevent_free(bev);
    }
    if (events & BEV_EVENT_EOF) {
        fprintf(stderr, "Close connection because out of data\n");
        bufferevent_free(bev);
    }
}

static void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen,void *ctx)
{
        /* We got a new connection! Set up a bufferevent for it. */
    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev, echo_read_cb, echo_write_cb, echo_event_cb, NULL);

    bufferevent_enable(bev, EV_READ|EV_WRITE);
}

static void accept_error_cb(struct evconnlistener *listener, void *ctx)
{
    struct event_base *base = evconnlistener_get_base(listener);
    int err = EVUTIL_SOCKET_ERROR();
    fprintf(stderr, "Got an error %d (%s) on the listener. Shutting down.\n", err, evutil_socket_error_to_string(err));

    event_base_loopexit(base, NULL);
}

int main(int argc, char **argv)
{
    struct event_base *base;
    struct evconnlistener *listener;
    struct sockaddr_in sin;

    if (PORT_USE<=0 || PORT_USE>65535) {
        puts("Invalid port");
        return 1;
    }
    base = event_base_new();
    if (!base) {
        puts("Couldn't open event base");
        return 1;
    }
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;

    inet_aton("127.0.0.1", &sin.sin_addr);
    sin.sin_port = htons(PORT_USE);
    listener = evconnlistener_new_bind(base, accept_conn_cb, NULL, LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE, -1, (struct sockaddr*)&sin, sizeof(sin));
    if (!listener) {
        perror("Couldn't create listener");
        return 1;
    }
    evconnlistener_set_error_cb(listener, accept_error_cb);
    event_base_dispatch(base);
    return 0;
}