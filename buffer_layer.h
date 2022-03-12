//
// Created by Karl_ on 2/28/2022.
//

#ifndef LAYERS_BUFFER_LAYER_H
#define LAYERS_BUFFER_LAYER_H

#include "layer_definitions.h"

typedef uint32_t hdr_reserved_t;
typedef uint16_t hdr_len_t;

typedef struct buffer_layer_message_t {
    hdr_reserved_t reserved;
    hdr_len_t message_len;
    uint8_t *content;
    hdr_len_t len_consumed;
} buffer_layer_message_t;


typedef struct buffer_layer_t {
    resource_layer_t;
    buffer_layer_message_t *buf_message;
} buffer_layer_t;

layer_t *buffer_layer_make(void *resource, void *params);

#endif //LAYERS_BUFFER_LAYER_H
