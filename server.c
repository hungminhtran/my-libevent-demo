#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


static void bev_read_cb(struct bufferevent *bev, void *ctx) {
    struct evbuffer *input_evb = bufferevent_get_input(bev);
    
}

static void bev_event_cb(struct bufferevent *bev, short ev, void *ctx) {
    switch (ev) {
        case BEV_EVENT_ERROR:
        fprintf(stderr, "Error from bufferevent");
        bufferevent_free(bev);
        break;
        case BEV_EVENT_EOF:
        fprintf(stderr, "Close connection EOF\n");
        bufferevent_free(bev);
        break;
        default:;
    }

}
static void listenerCallbackAcceptConnection(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen,void *ctx) {
    struct event_base *base = evconnlistener_get_eventbase(listener);
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, bev_read_cb, bev_write_cb, bev_event_cb, NULL);
    bufferevent_enable(bev, EV_READ|EV_WRITE);    

}
static void listener_error_cb(struct evconnlistener *evl, void *ctx) {
    int err = EVUTIL_SOCKET_ERROR();
    fprintf(stderr, "Got an error:(%d) %s", err, evutil_socket_error_to_string(err));
    exit(1);
}
int main(void) {
    struct event_base *base;
    struct eventlistener *listener;
    struct sockaddr_in sock;

    int port = 8000;
    char * ip = "127.0.0.1";

    base = event_base_new();
    if (!base) {
        fprintf(stderr, "Create event base failure");
        exit(1);
    }
    sock.sin_family = AF_INET; //ipv4
    sock.sin_addr.s_addr = inet_addr(ip);
    sock.sin_port = htons(port);

    listener = evconnlistener(base, listenerCallbackAcceptConnection, NULL, LEV_OPT_CLOSE_ON_FREE |  LEV_OPT_REUSEABLE, 1024, sock, sizeof(sock));
    if (!listener) {
        fprintf(stderr, "Create listener failure");
        exit(1);
    }
    evconnlistener_set_error_cb(listener, listener_error_cb);
    event_dispatch_base(base);
    exit(0);
}
