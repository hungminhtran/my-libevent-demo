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

#include <syslog.h>
#include <time.h>

#define PORT_USE 8888
#define DB_ENV "./demoDB"
#define DB_NAME "what"

int TOTAL_CONNECTION = 0;
int ORDER = 0;
int TOTAL_REQUEST = 0;

clock_t BEGIN_TIME, ENG_TIME;
double time_spent;


static void echo_read_cb(struct bufferevent *bev, void *ctx)
{

    struct evbuffer *input = bufferevent_get_input(bev);
    struct evbuffer *output = bufferevent_get_output(bev);
    //copy input data to keyVal
    size_t len = evbuffer_get_length(input);
    char *keyVal;
    keyVal = malloc(MAX_KEY_ALLOCATE_SIZE);
    memset(keyVal, 0, MAX_KEY_ALLOCATE_SIZE);
    ev_ssize_t tlen = evbuffer_remove(input, keyVal, len);
    if (tlen < 0) {
        syslog(LOG_INFO, "error when copy input dat");
        syslog(LOG_INFO, "***input data %s\n", keyVal);
    }
    //get output data from database
    char *data;
    data = malloc(MAX_DATA_ALLOCATE_SIZE);
    getDataFromDB(DB_ENV, DB_NAME, keyVal[0], &data);

    if (strlen(data) > 0)
        //passing data to write cb
        // evbuffer_add_printf(output, data, MAX_DATA_ALLOCATE_SIZE);
        evbuffer_add(output, data, MAX_DATA_ALLOCATE_SIZE);
        // evbuffer_add_printf(output,"%d %s", MAX_DATA_ALLOCATE_SIZE, data);
    else
        evbuffer_add_printf(output,"%d %s", 2, "NF");

    free(data);
    free(keyVal);
}

static void echo_write_cb(struct bufferevent *bev, void *ctx)
{
}

static void echo_event_cb(struct bufferevent *bev, short events, void *ctx)
{
    if (events & BEV_EVENT_ERROR ) {
        syslog(LOG_INFO, "Close connection because of error\n");
        bufferevent_free(bev);
    }
    else if (events & BEV_EVENT_EOF) {
        // syslog(LOG_INFO, "Close connection because out of data\n");
        bufferevent_free(bev);
    }
    TOTAL_CONNECTION--;
    ORDER++;
    fprintf(stdout, "%d. total connection f %d\n", ORDER, TOTAL_CONNECTION);
    if (TOTAL_CONNECTION == 0) {
        /* here, do your time-consuming job */
        ENG_TIME = clock();
        time_spent = (double)(ENG_TIME - BEGIN_TIME) / CLOCKS_PER_SEC;
        fprintf(stdout, "total time: %f total request: %d request/second %f", time_spent, TOTAL_REQUEST, TOTAL_REQUEST/time_spent);
    }
}

static void accept_conn_cb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *address, int socklen,void *ctx)
{
    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev, echo_read_cb, echo_write_cb, echo_event_cb, NULL);

    bufferevent_enable(bev, EV_READ|EV_WRITE);
    // syslog(LOG_INFO, "Accpet new connection\n");
    if (!TOTAL_CONNECTION) {
        BEGIN_TIME = clock();
        TOTAL_REQUEST = 0;
    }
    TOTAL_CONNECTION++;
    ORDER++;
    TOTAL_REQUEST++;
    fprintf(stdout, "%d. total connection a %d\n", ORDER, TOTAL_CONNECTION);
}

static void accept_error_cb(struct evconnlistener *listener, void *ctx)
{
    struct event_base *base = evconnlistener_get_base(listener);
    int err = EVUTIL_SOCKET_ERROR();
    syslog(LOG_INFO, "Got an error %d (%s) on the listener. Shutting down.\n", err, evutil_socket_error_to_string(err));

    event_base_loopexit(base, NULL);
}

int main(int argc, char **argv)
{
    openlog("demo-server-libevent", LOG_PID|LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "initializing");
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
    if (argc < 2) {
        fprintf(stdout, "server.out <IP>\n");
        return 1;
    }
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;

    inet_aton(argv[1], &sin.sin_addr);
    fprintf(stdout, "ip: %s\n", argv[1]);
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
