//
// Created by Karl_ on 2/28/2022.
//

#ifndef LAYERS_FACTORY_H
#define LAYERS_FACTORY_H

#include "layer_definitions.h"

struct layer_factory_t;

/**
 * Make the next layer in the stack using the previous layer.
 * @param {prev_layer} The previous layer in the stack. Type layer_t *
 * @return {layer_t *} A pointer to the next layer in the stack
 */
typedef layer_t *(*make_with_func)(void *prev_layer, void *params);

/**
 * Set next factory at the end of list of factories
 * @param {self} the owner of the set_next_factory
 * @param {next} The factory to set next
 * @return {struct layer_factory_t *} returns self
 */
typedef struct layer_factory_t *(*set_next_factory_func)(struct layer_factory_t *self, struct layer_factory_t *next);

typedef void (*destroy_factory_func)(void **factory);

typedef struct layer_factory_t {
    layer_t *(*make_with)(void *self, void *prev_layer);

    make_with_func _maker;
    struct layer_factory_t *_next_factory;
    set_next_factory_func set_next_factory;
    destroy_factory_func destroy;
    void *maker_func_params;
} layer_factory_t;

layer_factory_t *make_factory(make_with_func maker_func, void *maker_func_params);


#endif //LAYERS_FACTORY_H
