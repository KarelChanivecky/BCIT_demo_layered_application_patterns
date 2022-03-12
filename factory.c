//
// Created by Karl_ on 2/28/2022.
//

#include "factory.h"
#include "common.h"
#include "stdlib.h"


layer_t *make(void *self, void *prev_layer) {
    RETURN_IF(self == NULL, NULL);
    RETURN_IF(prev_layer == NULL, NULL);

    layer_factory_t *factory = self;
    layer_t *this = factory->_maker(prev_layer, factory->maker_func_params);
    if (factory->_next_factory) {
        layer_t *next = factory->_next_factory->make_with(factory->_next_factory, this);
        if (!next) {
            this->destroy((void **) &this);
            return NULL;
        }
        return next;
    }
    return this;
}


layer_factory_t *set_next_factory(struct layer_factory_t *self, struct layer_factory_t *next) {
    RETURN_IF(self == NULL, NULL);
    RETURN_IF(next == NULL, NULL);
    layer_factory_t *factory_iter = self;

    while (factory_iter->_next_factory != NULL) {
        factory_iter = factory_iter->_next_factory;
    }

    factory_iter->_next_factory = next;

    return self;
}


void destroy_factory(void **factory_dptr) {
    if (factory_dptr == NULL) {
        return;
    }

    layer_factory_t *factory = (layer_factory_t *) *factory_dptr;

    if (factory == NULL) {
        return;
    }

    if (factory->_next_factory != NULL) {
        factory->_next_factory->destroy((void **) &(factory->_next_factory));
    }

    factory->set_next_factory = NULL;
    factory->destroy = NULL;
    factory->make_with = NULL;
    free(factory);
    *factory_dptr = NULL;
}

layer_factory_t *make_factory(make_with_func maker_func, void *maker_func_params) {
    layer_factory_t *factory = (layer_factory_t *) malloc(sizeof(layer_factory_t));
    TERMINATE_IF(factory == NULL, "failed to malloc when making factory", ERR_MEM);
    factory->make_with = make;
    factory->_maker = maker_func;
    factory->_next_factory = NULL;
    factory->set_next_factory = set_next_factory;
    factory->destroy = destroy_factory;
    return factory;
};




