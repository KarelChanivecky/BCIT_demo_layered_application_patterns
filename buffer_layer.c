//
// Created by Karl_ on 2/28/2022.
//

#include "buffer_layer.h"

#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "dlinked_list.h"

void read_header(buffer_layer_t *self, buffer_layer_message_t *msg) {
    uint8_t header_size = sizeof(hdr_len_t) + sizeof(hdr_reserved_t);
    uint8_t bytes_read = 0;
    uint8_t header_buff[header_size];
    resource_layer_t *resource = (resource_layer_t *) self->resource;
    while (bytes_read < header_size) {
        bytes_read += resource->read(resource, header_buff + bytes_read, header_size + bytes_read);
    }

    msg->message_len = ntohs(*(hdr_len_t *) header_buff);
    msg->reserved = ntohl(*(hdr_reserved_t *) (header_buff + sizeof(hdr_len_t)));
}

void read_message(buffer_layer_t *self, buffer_layer_message_t *msg) {
    read_header(self, msg);
    hdr_len_t total_bytes_read = 0;
    hdr_len_t bytes_read = 0;
    resource_layer_t *resource = (resource_layer_t *) self->resource;

    uint8_t *message_buffer = (uint8_t *) malloc(msg->message_len);
    while (bytes_read < msg->message_len) {
        bytes_read = resource->read(resource, message_buffer + bytes_read, msg->message_len + bytes_read);
        total_bytes_read += bytes_read;
    }

    msg->content = message_buffer;
    msg->len_consumed = 0;
}


size_t buffer_layer_read(void *vself, uint8_t *buf, size_t bytes_to_read) {
    TERMINATE_IF(vself == NULL, "self is null ptr", INVALID_ARG);
    TERMINATE_IF(buf == NULL, "buf is null ptr", INVALID_ARG);
    buffer_layer_t *self = (buffer_layer_t *) vself;
    TERMINATE_IF(buf == NULL, "layer->self is null ptr", ERR_NULL_PTR);
    uint16_t total_read = 0;
    uint16_t buf_index = 0;
    buffer_layer_message_t *message;
    dlinked_list *message_list = dlinked_create_list();
    TERMINATE_IF(message_list == NULL, "Failed to allocate message_list", ERR_MEM);

    if (self->buf_message) {
        dlinked_push(message_list, self->buf_message);
        total_read += self->buf_message->message_len - self->buf_message->len_consumed;
    }

    while (total_read < bytes_to_read) {
        message = (buffer_layer_message_t *) malloc(sizeof(buffer_layer_message_t));
        read_message(self, message);
        dlinked_push(message_list, message);
        total_read += message->message_len;
    }

    while (message_list->size > 0) {
        // Not necessary to check for buf_index as the list should have just enough bytes to fill the requirement

        message = dlinked_pop_head(message_list);
        hdr_len_t bytes_to_consume;
        size_t bytes_read = buf_index + message->message_len;

        if (bytes_read > bytes_to_read) {
            // take only the necessary bytes to meet the requirement
            bytes_to_consume = bytes_to_read - buf_index;
        } else {
            // take the remaining bytes in the message
            bytes_to_consume = message->message_len - message->len_consumed;
        }

        message->len_consumed = bytes_to_consume;
        memcpy(buf + buf_index, message->content, bytes_to_consume);
        if (bytes_to_consume == message->message_len) {
            free(message->content);
            free(message);
            message = NULL;
        } else {
            self->buf_message = message;
        }

        buf_index += bytes_to_consume;
    }

    dlinked_free_list(&message_list);
    return bytes_to_read;
}

ssize_t buffer_layer_write(void *vself, uint8_t *buf, size_t bytes_to_write) {
    TERMINATE_IF(vself == NULL, "vself is null ptr", INVALID_ARG);
    TERMINATE_IF(buf == NULL, "buf is null ptr", INVALID_ARG);

    resource_layer_t *self = (resource_layer_t *) vself;
    TERMINATE_IF(self->resource == NULL, "layer->self is null ptr", ERR_NULL_PTR);
    resource_layer_t *resource = (resource_layer_t *) self->resource;
    ssize_t header_size = sizeof(hdr_len_t) + sizeof(hdr_reserved_t);
    uint8_t send_buff[header_size + bytes_to_write];
    uint32_t reserved = 0;
    hdr_len_t net_message_length = htons(bytes_to_write);
    size_t bytes_used = 0;
    memcpy(send_buff, &net_message_length, sizeof(hdr_len_t));
    bytes_used += sizeof(hdr_len_t);
    memcpy(send_buff + bytes_used, &reserved, sizeof(hdr_reserved_t));
    bytes_used += sizeof(hdr_reserved_t);
    memcpy(send_buff + bytes_used, buf, bytes_to_write);
    ssize_t ret = resource->write(resource, send_buff, header_size + bytes_to_write);
    RETURN_IF(ret == -1, ret);
    return ret - header_size;
}

void buffer_layer_destroy(void **vself) {
    buffer_layer_t *self = *(buffer_layer_t **) vself;

    if (!put(self)) {
        return;
    }

    if (self->buf_message) {
        if (self->buf_message) {
            free(self->buf_message->content);
            free(self->buf_message);
        }
    }
    resource_layer_t *resource = (resource_layer_t *) self->resource;
    resource->destroy(&self->resource);
}


layer_t *buffer_layer_make(void *resource, void *params) {
    buffer_layer_t *layer = (buffer_layer_t *) base_layer_make(sizeof(buffer_layer_t), resource);
    layer->read = buffer_layer_read;
    layer->write = buffer_layer_write;
    layer->destroy = buffer_layer_destroy;
    layer->buf_message = NULL;
    return (layer_t *) layer;
}

