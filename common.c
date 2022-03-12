//
// Created by Karl_ on 3/10/2022.
//

#include <string.h>
#include "common.h"

void log_message(FILE *file, const char *caller, const char *message) {
    if (caller == NULL) {
        fprintf(file, "%s\n", message);
        return;
    }
    fprintf(file, "%s: %s\n", caller, message);
}

void log_err(const char *caller, const char *message, int error_code) {
    if (caller == NULL) {
        if (error_code == ERR_ERRNO) {
            fprintf(stderr, "ERROR={%d} errno={%d}:%s  %s\n", error_code, errno, strerror(errno), message);
            return;
        }
        fprintf(stderr, "ERROR={%d}  %s\n", error_code, message);
        return;
    }
    if (error_code == ERR_ERRNO) {
        fprintf(stderr, "%s ERROR={%d} errno={%d}:%s  %s\n", caller, error_code, errno, strerror(errno), message);
        return;
    }
    fprintf(stderr, "%s: ERROR={%d}  %s\n", caller, error_code, message);
}

void terminate(const char *caller_name, const char *message, const int code) {
    log_err(caller_name, message, code);
    exit(code);
}