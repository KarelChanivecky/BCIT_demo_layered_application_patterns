//
// Created by Karl_ on 2/18/2022.
//

#ifndef LAYERS_LAYER_DEFINITIONS_H
#define LAYERS_LAYER_DEFINITIONS_H

#include <stdlib.h>
#include <stdint.h>
#include "common.h"
#include "event_loop.h"

/**
 * Read from source
 * @param {self} The owner of the read_func, a pointer to a layer_t struct
 * @param {buff} a buffer of at least size bytes_to_read
 * @param {bytes_to_read} number of bytes to read
 * @return {size_t} the number of bytes actually read
 */
typedef size_t (*read_func)(void *self, uint8_t *buff, size_t bytes_to_read);

/**
 * Write to source
 * @param {self} The owner of the write_func, a pointer to a layer_t struct
 * @param {buff} a buffer of at least size bytes_to_write
 * @param {bytes_to_write} number of bytes to write
 * @return {size_t} the number of bytes actually written
 */
typedef ssize_t  (*write_func)(void *self, uint8_t *buff, size_t bytes_to_write);

/**
 * Close the layer
 * @param {self} The struct owner of the function pointer
 */
typedef void (*close_func)(void *self);


struct listener_layer_t;

typedef void (*set_listener_func)(void *self, struct listener_layer_t *listener);

struct layer_t;

typedef event_listener_t *(*to_event_listener_func)(struct layer_t *self);

typedef struct layer_t {
    referrable_t;
    void *resource;
    to_event_listener_func to_event_listener;
    close_func close;
} layer_t;

typedef struct resource_layer_t {
    layer_t;
    write_func write;
    read_func read;
} resource_layer_t;


struct listener_layer_t;

/**
 * Pass message to listener.
 * @param {self} The owner of the handle_error_func
 * @param {message} corresponding type
 */
typedef ssize_t (*pass_message_func)(void *self, void *message);

typedef struct emitter_layer_t {
    layer_t;
    struct listener_layer_t *listener;
    set_listener_func set_listener;
    pass_message_func send_message;
} emitter_layer_t;


typedef enum comm_error_t {
    COMM_ERR_NOT_READY,
    COMM_ERR_NOT_AVAILABLE,
    COMM_ERR_NOT_ALLOWED,
    COMM_ERR_INVALID_SYNTAX,
    COMM_ERR_INCOMPLETE,
    COMM_ERR_BROKEN_CONNECTION,
    ETC_ETC_ETC,

} comm_error_t;

/**
 * Handle an error.
 * @param {self} The owner of the handle_error_func
 * @param {error_code} an error code of type error_t
 */
typedef void (*handle_error_func)(void *self, comm_error_t error_code);


typedef struct listener_layer_t {
    layer_t;
    pass_message_func pass_message;
    handle_error_func handle_error;
} listener_layer_t;

void base_layer_destroy(void **vself);

layer_t *base_layer_make(size_t layer_size, void *resource);

event_listener_t *base_to_event_listener(layer_t *self);

#endif //LAYERS_LAYER_DEFINITIONS_H
