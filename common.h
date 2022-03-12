//
// Created by Karl_ on 2/18/2022.
//

#ifndef LAYERS_COMMON_H
#define LAYERS_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

enum termination_codes {
    ERR_MEM = 1,
    INVALID_ARG,
    INVALID_OBJ_STATE,
    ERR_NULL_PTR,
    ERR_ERRNO
};

void log_message(FILE *file, const char *caller, const char *message);

void log_err(const char *caller, const char *message, int error_code);

#define DEB_LOG(message) do {               \
    if (DEB_LOG_ENABLED) {                  \
        log_message(stdout, (__func__), (message));  \
    }                                       \
} while (0);


void terminate(const char *caller_name, const char *message, int code);

#define RETURN_IF(predicate, retval) if ((predicate)) return (retval)

#define TERMINATE_IF(predicate, message, code) do { \
        if ((predicate)) {                          \
            terminate((__func__), (message), (code));           \
        }                                           \
    } while (0)


typedef void (*destroy_func)(void **self);

typedef struct referable_t {
    uint32_t refc;
    destroy_func destroy;
} referrable_t;

#define hold(referrable)((referrable))->refc++

#define put(referrable) (--(referrable)->refc <= 0)


#endif //LAYERS_COMMON_H
