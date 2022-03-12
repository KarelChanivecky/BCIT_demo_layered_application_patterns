//
// Created by Karl_ on 2/28/2022.
//

#include "cipher_layer.h"


#include <string.h>
#include "common.h"
#include "dlinked_list.h"

typedef uint8_t cipher_key_t;

static cipher_key_t cipher_key_i = 1;

size_t cipher_layer_read(void *vself, uint8_t *buf, size_t bytes_to_read) {
    TERMINATE_IF(vself == NULL, "vself is null ptr", INVALID_ARG);
    TERMINATE_IF(buf == NULL, "buf is null ptr", INVALID_ARG);
    layer_t *self = (layer_t *) vself;
    TERMINATE_IF(self->resource == NULL, "self->resource is null ptr", ERR_NULL_PTR);
    resource_layer_t *resource = (resource_layer_t *) self->resource;
    uint8_t l2_buff[sizeof(cipher_key_t) + bytes_to_read];
    resource->read(resource, l2_buff, sizeof(cipher_key_i) + bytes_to_read);
    cipher_key_t cipher_key = l2_buff[0];
    for (int i = sizeof(cipher_key_t); i < bytes_to_read + sizeof(cipher_key_t); i++) {
        l2_buff[i] -= cipher_key;
    }

    memcpy(buf, l2_buff + sizeof(cipher_key_t), bytes_to_read);

    return bytes_to_read;
}

ssize_t cipher_layer_write(void *vself, uint8_t *buf, size_t bytes_to_write) {
    TERMINATE_IF(vself == NULL, "vself is null ptr", INVALID_ARG);
    TERMINATE_IF(buf == NULL, "buf is null ptr", INVALID_ARG);

    layer_t *self = (layer_t *) vself;
    TERMINATE_IF(self->resource == NULL, "layer->resource is null ptr", ERR_NULL_PTR);
    resource_layer_t *resource = (resource_layer_t *) self->resource;
    uint8_t cipher_key = cipher_key_i++;

    // we should do it in one go, but trying to demonstrate how the buffer layer works. This should work
    ssize_t ret = resource->write(resource, &cipher_key, sizeof(cipher_key));
    RETURN_IF(ret == -1, ret);

    for (int i = 0; i < bytes_to_write; i++) {
        buf[i] += cipher_key;
    }

    return resource->write(resource, buf, bytes_to_write);
}


layer_t *cipher_layer_make(void *resource, void *params) {
    TERMINATE_IF(resource == NULL, "resource is null", INVALID_ARG);

    resource_layer_t *layer = (resource_layer_t *) base_layer_make(sizeof(resource_layer_t), resource);
    layer->read = cipher_layer_read;
    layer->write = cipher_layer_write;
    return (layer_t *) layer;
}