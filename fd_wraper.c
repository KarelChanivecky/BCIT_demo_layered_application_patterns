//
// Created by Karl_ on 2/28/2022.
//

#include <sys/unistd.h>
#include "fd_wraper.h"
#include "common.h"


size_t fd_read(void *self, uint8_t *buf, size_t bytes_to_read) {
    TERMINATE_IF(self == NULL, "self is null ptr", INVALID_ARG);
    TERMINATE_IF(buf == NULL, "buf is null ptr", INVALID_ARG);

    resource_layer_t *layer = (resource_layer_t *) self;
    TERMINATE_IF(buf == NULL, "layer->context is null ptr", ERR_NULL_PTR);
    fd_resource_t *resource = (fd_resource_t *) layer->resource;

    return read(resource->in_fd, buf, bytes_to_read);
}

ssize_t fd_write(void *self, uint8_t *buf, size_t bytes_to_write) {
    TERMINATE_IF(self == NULL, "self is null ptr", INVALID_ARG);
    TERMINATE_IF(buf == NULL, "buf is null ptr", INVALID_ARG);

    resource_layer_t *layer = (resource_layer_t *) self;
    TERMINATE_IF(layer->resource == NULL, "layer->resource is null ptr", ERR_NULL_PTR);
    fd_resource_t *resource = (fd_resource_t *) layer->resource;

    return write(resource->out_fd, buf, bytes_to_write);
}

void fd_close(void *self_ptr) {
    resource_layer_t *layer = self_ptr;
    TERMINATE_IF(layer == NULL, "layer is null ptr", INVALID_ARG);
    fd_resource_t *resource = (fd_resource_t *) layer->resource;
    close(resource->in_fd);
    close(resource->out_fd);
}

event_listener_t *fd_layer_to_event_listener(layer_t *self) {
    fd_resource_t *resource = (fd_resource_t *) self->resource;
    event_listener_t *listener = event_listener_make(self, resource->in_fd, NULL);
    return listener;
}


layer_t *fd_layer_make(void *vfd_resource, void *params) {
    TERMINATE_IF(vfd_resource == NULL, "fd_context is null", INVALID_ARG);
    resource_layer_t *layer = (resource_layer_t *) base_layer_make(sizeof(resource_layer_t), vfd_resource);
    layer->read = fd_read;
    layer->write = fd_write;
    layer->close = fd_close;
    layer->to_event_listener = fd_layer_to_event_listener;
    return (layer_t *) layer;
};

void fd_resource_destroy(void **vself) {
    fd_resource_t *self = *(fd_resource_t **) vself;
    if (!put(self)) {
        return;
    }
    free(self);
    *vself = NULL;
}

fd_resource_t *fd_resource_make(int in_fd, int out_fd) {
    fd_resource_t *resource = (fd_resource_t *) malloc(sizeof(fd_resource_t));
    TERMINATE_IF(resource == NULL, "failed to malloc when making fd_resource_t", ERR_MEM);
    resource->in_fd = in_fd;
    resource->out_fd = out_fd;
    resource->destroy = fd_resource_destroy;
    return resource;
}
