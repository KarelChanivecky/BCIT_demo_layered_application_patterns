//
// Created by Karl_ on 3/3/2022.
//

#include "layer_definitions.h"

void base_layer_destroy(void **vself) {
    struct layer_t *layer = *(struct layer_t **) vself;
    if (!put(layer)) {
        return;
    }
    if (layer->resource) {
        referrable_t *resource = (referrable_t *) layer->resource;
        resource->destroy(&layer->resource);
    }
    layer->to_event_listener = NULL;
    layer->destroy = NULL;
    layer->close = NULL;
    free(layer);
    *vself = NULL;
}


void base_layer_close(void *vself) {
    TERMINATE_IF(vself == NULL, "vself is null ptr", INVALID_ARG);

    layer_t *self = (layer_t *) vself;
    layer_t *prev = (layer_t *) self->resource;
    prev->close(prev);
    // Do not destroy here! Close and destroy are two different things, and they
    // should be initiated from the top layer
}

event_listener_t *base_to_event_listener(layer_t *self) {
    layer_t *prev = (layer_t *) self->resource;
    event_listener_t *event_listener = prev->to_event_listener(prev);
    return event_listener;
}


layer_t *base_layer_make(size_t layer_size, void *resource) {
    // technically this could restrict re-usability, but ok in this case. Move to caller func if any issues
    RETURN_IF(resource == NULL, NULL);

    layer_t *layer = (layer_t *) malloc(layer_size == 0 ? sizeof(layer_t) : layer_size);
    TERMINATE_IF(layer == NULL, "failed to malloc when making layer 1", ERR_MEM);
    layer->destroy = base_layer_destroy;
    layer->to_event_listener = base_to_event_listener;
    layer->resource = resource;
    hold((referrable_t *) resource);
    return layer;
}

