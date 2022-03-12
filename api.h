//
// Created by Karl_ on 2/18/2022.
//

#ifndef LAYERS_API_H
#define LAYERS_API_H

#include "stdlib.h"
#include "layer_definitions.h"


typedef struct api_t {
    listener_layer_t;

    int (*say_hi)(void *vself);

    int (*send_not_ok_text)(void *vself, size_t len, char *text);
} api_t;

layer_t *api_make(void *vresource, void *params);

#endif //LAYERS_API_H
