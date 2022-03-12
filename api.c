//
// Created by Karl_ on 2/18/2022.
//

#include <string.h>
#include <time.h>
#include "api.h"
#include "api_message.h"

int api_say_hi(void *vself) {
    listener_layer_t *self = (listener_layer_t *) vself;
    emitter_layer_t *resource = (emitter_layer_t *) self->resource;
    char *hi_text = "hello friend";
    printf("Sending: %s\n", hi_text);
    ok_message_t *hi_message = api_ok_message_make(strlen(hi_text), hi_text);
    return resource->send_message(resource, hi_message) == 0 ? 0 : 1;
}

int api_send_not_ok_text(void *vself, size_t len, char *text) {
    printf("Sending: %s\n", text);
    listener_layer_t *self = (listener_layer_t *) vself;
    emitter_layer_t *resource = (emitter_layer_t *) self->resource;
    not_ok_message_t *message = api_not_ok_message_make(len, text, time(NULL));
    return resource->send_message(resource, message) == 0 ? 0 : 1;
}

int api_answer_message(void *vself, void *vincoming_message) {
    api_message_t *incoming_message = (api_message_t *) vincoming_message;
    char *bad_text_1 = "I am not your friend, pal";
    char *bad_text_2 = "I am not your pal, buddy";
    char *bad_text_3 = "I am not your buddy, friend";

    if (incoming_message->header->text_type == API_MESSAGE_NOT_OK) {
        if (0 == strncmp(bad_text_1, (char *) incoming_message->text, incoming_message->header->text_len)) {
            return api_send_not_ok_text(vself, strlen(bad_text_2), bad_text_2) != 0;
        }
        if (0 == strncmp(bad_text_2, (char *) incoming_message->text, incoming_message->header->text_len)) {
            return api_send_not_ok_text(vself, strlen(bad_text_3), bad_text_3) != 0;
        }
        return api_send_not_ok_text(vself, strlen(bad_text_1), bad_text_1) != 0;
    }

    return api_send_not_ok_text(vself, strlen(bad_text_1), bad_text_1);
}

ssize_t api_handle_message(void *vself, void *vmessage) {
    api_message_t *message = (api_message_t *) vmessage;
    hold(message);

    printf("Receiving: %s\n", message->text);

    // more logic here as needed
    int ret = api_answer_message(vself, vmessage);
    put(message);
    return ret;
}

void api_handle_error(void *vself, comm_error_t error_code) {
    printf("Received error code %d\n", error_code);
    printf("%s", strerror(errno));
    listener_layer_t *self = (listener_layer_t *) vself;
    self->close((void **) &self);
}

layer_t *api_make(void *vresource, void *params) {
    api_t *api = (api_t *) base_layer_make(sizeof(listener_layer_t), vresource);
    emitter_layer_t * resource = (emitter_layer_t*) vresource;
    resource->set_listener(resource, (struct listener_layer_t *) api);
    api->handle_error = api_handle_error;
    api->pass_message = api_handle_message;
    api->say_hi = api_say_hi;
    api->send_not_ok_text = api_send_not_ok_text;
    return (layer_t *) api;
}

