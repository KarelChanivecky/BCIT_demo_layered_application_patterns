//
// Created by Karl_ on 2/28/2022.
//

#ifndef LAYERS_FD_WRAPPER_H
#define LAYERS_FD_WRAPPER_H

#include "layer_definitions.h"

typedef struct fd_resource_t {
    referrable_t;
    int in_fd;
    int out_fd;
} fd_resource_t;

fd_resource_t *fd_resource_make(int in_fd, int out_fd);

layer_t *fd_layer_make(void *fd_resource, void *params);

#endif //LAYERS_FD_WRAPPER_H
