//
// Created by Karl_ on 2/28/2022.
//

#include "api_message_layer.h"

#include "common.h"
#include "dlinked_list.h"
#include "api_message.h"

api_message_header_t *api_message_read_header(emitter_layer_t *self) {
    resource_layer_t *resource = (resource_layer_t *) self->resource;
    uint8_t buffer[sizeof(api_message_header_t)];
    size_t bytes_read = resource->read(resource, buffer, API_MSG_HDR_SIZE);
    // warning, can't just do sizeof(api_message_header_t) because the size includes all the func pointers and likely
    // padding as well
    if (bytes_read > API_MSG_HDR_SIZE) {
        self->listener->handle_error(self->listener, COMM_ERR_INCOMPLETE);
    }
    return api_message_header_deserialize(buffer);
}

api_message_t *api_message_read_ok_message(emitter_layer_t *self, api_message_header_t *header) {
    resource_layer_t *resource = (resource_layer_t *) self->resource;
    uint8_t buffer[header->text_len];
    size_t bytes_read = resource->read(resource, buffer, header->text_len);
    if (bytes_read != header->text_len) {
        return NULL;
    }
    return (api_message_t *) api_ok_message_deserialize(header, buffer);
}

api_message_t *api_message_read_not_ok_message(emitter_layer_t *self, api_message_header_t *header) {
    resource_layer_t *resource = (resource_layer_t *) self->resource;
    size_t total_size = header->text_len + sizeof(timestamp_t);
    uint8_t buffer[total_size];
    size_t bytes_read = resource->read(resource, buffer, total_size);
    if (bytes_read != total_size) {
        return NULL;
    }
    return (api_message_t *) not_ok_message_deserialize(header, buffer);
}

ssize_t api_message_send_message(void *vself, void *vmessage) {
    emitter_layer_t *self = (emitter_layer_t *) vself;
    resource_layer_t *resource = (resource_layer_t *) self->resource;
    api_message_t *message = (api_message_t *) vmessage;
    uint8_t buffer[message->size];
    message->serialize(message, buffer);

    // Must write as we read. Else the next layers may get confused. header -> body
    ssize_t ret = resource->write(resource, buffer, API_MSG_HDR_SIZE);

    // not that this is any better than returning ret. Just demonstrating different ideas.
    if (ret == -1) {
        // we would also be going over errno to see what would be the most appropriate error code
        self->listener->handle_error(self->listener, COMM_ERR_BROKEN_CONNECTION);
    }

    ret = resource->write(resource , buffer + API_MSG_HDR_SIZE, message->size - API_MSG_HDR_SIZE);
    if (ret == -1) {
        self->listener->handle_error(self->listener, COMM_ERR_BROKEN_CONNECTION);
    }
    return ret;
}

typedef api_message_t *(*read_message_func)(emitter_layer_t *self, api_message_header_t *header);

static read_message_func read_message[] = {
        [API_MESSAGE_OK] = api_message_read_ok_message,
        [API_MESSAGE_NOT_OK] = api_message_read_not_ok_message,
};

void api_message_trigger(void *self_ptr) {
    emitter_layer_t *self = (emitter_layer_t *) self_ptr;
    api_message_header_t *header = api_message_read_header(self);
    if (!header) {
        // in a real scenario we would be checking errno to understand what happened.
        self->listener->handle_error(self->listener, COMM_ERR_BROKEN_CONNECTION);
    }
    api_message_t *message = read_message[header->text_type](self, header);
    if (!message) {
        // in a real scenario we would be checking errno to understand what happened.
        self->listener->handle_error(self->listener, COMM_ERR_BROKEN_CONNECTION);
    }
    self->listener->pass_message(self->listener, message);
}

void api_message_set_listener(void *vself, listener_layer_t *listener) {
    ((emitter_layer_t *) vself)->listener = listener;
}

void api_message_layer_destroy(void **vself) {
    emitter_layer_t *self = *(emitter_layer_t **) vself;
    self->set_listener = NULL;
    self->send_message = NULL;
    self->listener->destroy((void **) &self->listener);
    base_layer_destroy(vself);
}

layer_t *api_message_layer_make(void *resource, void *params) {
    TERMINATE_IF(resource == NULL, "resource is null", INVALID_ARG);

    emitter_layer_t *layer = (emitter_layer_t *) base_layer_make(sizeof(emitter_layer_t), resource);
    layer->send_message = api_message_send_message;
    layer->set_listener = api_message_set_listener;
    layer->destroy = api_message_layer_destroy;
    layer->listener = NULL;
    layer_t *prev = (layer_t *) resource;
    event_listener_t *event_listener = prev->to_event_listener(prev);
    event_listener->self = layer;
    event_listener->trigger = api_message_trigger;
    add_to_loop(event_listener);
    return (layer_t *) layer;
}