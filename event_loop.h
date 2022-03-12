//
// Created by Karl_ on 3/5/2022.
//

#ifndef LAYERS_EVENT_LOOP_H
#define LAYERS_EVENT_LOOP_H


#include "common.h"

typedef void (*trigger_func)(void *self);

typedef struct event_listener_t {
    referrable_t;
    int fd;
    trigger_func trigger;
    void *self;
} event_listener_t;

void add_to_loop(event_listener_t *listener);

_Noreturn int run_event_loop();

event_listener_t *event_listener_make(void *self, int fd, trigger_func trigger);


#endif //LAYERS_EVENT_LOOP_H
