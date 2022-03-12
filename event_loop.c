//
// Created by Karl_ on 3/5/2022.
//

#include <stddef.h>
#include <sys/select.h>
#include "event_loop.h"
#include "common.h"

#define MAX_LISTENERS 16

static event_listener_t *listeners[MAX_LISTENERS] = {NULL};


void add_to_loop(event_listener_t *listener) {
    listeners[listener->fd] = listener;
}

int preset_fd_set(fd_set *rfds) {
    FD_ZERO(rfds);
    int max = 0;
    for (int i = 0; i < MAX_LISTENERS; i++) {
        if (listeners[i]) {
            int cur_fd = listeners[i]->fd;
            FD_SET(cur_fd, rfds);
            max = max > cur_fd ? max : cur_fd;
        }
    }
    return max;
}

void check_fds(int set_count, fd_set *rfds) {
    for (int i = 0; i < MAX_LISTENERS; i++ && set_count > 0) {
        event_listener_t *listener = listeners[i];
        if (!listener) {
            continue;
        }

        if (FD_ISSET(listener->fd, rfds)) {
            listener->trigger(listener->self);
            set_count--;
        }
    }
}

_Noreturn int run_event_loop() {
    fd_set rfds;
    int retval;
    while (1) {
        // we could also add a timeout to select, then we could have "timeout" events where every so often
        // we check if the timeout has occurred. Out of scope for this app, however.
        int nfds = preset_fd_set(&rfds);

        retval = select(nfds + 1, &rfds, NULL, NULL, NULL);
        TERMINATE_IF(retval == -1, "select failed", ERR_ERRNO);

        check_fds(retval, &rfds);
    }
}

void event_listener_destroy(void **listener_ptr) {
    event_listener_t *listener = *(event_listener_t **) listener_ptr;
    if (!put(listener)) {
        return;
    }
    free(listener);
    *listener_ptr = NULL;
}

event_listener_t *event_listener_make(void *self, int fd, trigger_func trigger) {
    event_listener_t *listener = (event_listener_t *) malloc(sizeof(event_listener_t));
    TERMINATE_IF(listener == NULL, "failed to malloc when making listener", ERR_MEM);
    listener->fd = fd;
    listener->trigger = trigger;
    listener->refc = 0;
    listener->self = self;
    listener->destroy = event_listener_destroy;
    return listener;
}

