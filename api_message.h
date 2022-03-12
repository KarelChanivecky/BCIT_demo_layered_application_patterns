//
// Created by Karl_ on 3/5/2022.
//

#ifndef LAYERS_API_MESSAGE_H
#define LAYERS_API_MESSAGE_H

#include <stdint-gcc.h>
#include <stddef.h>
#include "common.h"

typedef uint8_t text_type_t_t;
typedef uint16_t text_len_t;
typedef uint32_t timestamp_t;

#define API_MSG_HDR_SIZE ((sizeof(text_type_t_t)) + (sizeof(text_len_t)))

typedef enum text_type_t {
    API_MESSAGE_OK,
    API_MESSAGE_NOT_OK,
} text_type_t;

typedef size_t (*serialize_func)(void *self, uint8_t *buf);

typedef struct api_message_header_t {
    referrable_t;
    text_len_t text_len;
    text_type_t text_type;
    serialize_func serialize;
} api_message_header_t;

typedef struct api_message_t {
    referrable_t;
    api_message_header_t *header;
    uint8_t *text;
    size_t size;
    serialize_func serialize;
} api_message_t;

typedef struct ok_message_t {
    api_message_t;
} ok_message_t;

typedef struct not_ok_message_t {
    api_message_t;
    uint32_t timestamp;
} not_ok_message_t;


api_message_header_t *api_message_header_deserialize(const uint8_t *bytes);

ok_message_t *api_ok_message_deserialize(api_message_header_t *header, uint8_t *bytes);

not_ok_message_t *not_ok_message_deserialize(api_message_header_t *header, uint8_t *bytes);

ok_message_t *api_ok_message_make(uint16_t len, char *text);

not_ok_message_t *api_not_ok_message_make(uint16_t len, const char *text, uint32_t timestamp);

#endif //LAYERS_API_MESSAGE_H
