//
// Created by Karl_ on 3/5/2022.
//

#include <string.h>
#include <netinet/in.h>
#include "api_message.h"

void api_message_header_destroy(void **header_ptr) {
    api_message_header_t *header = *(api_message_header_t **) header_ptr;
    if (!put(header)) {
        return;
    }
    free(header);
    *header_ptr = NULL;
}

void api_message_destroy(void **message_ptr) {
    api_message_t *message = *(api_message_t **) message_ptr;
    if (!put(message)) {
        return;
    }
    message->header->destroy((void **) &(message->header));
    free(message->text);
    *message_ptr = NULL;
}

size_t api_header_serialize(void *self, uint8_t *buffer) {
    api_message_header_t *header = (api_message_header_t *) self;
    size_t used_index = 0;
    buffer[0] = header->text_type;
    used_index += sizeof(uint8_t);
    uint16_t net_text_size = htons(header->text_len);
    memcpy(buffer + used_index, &net_text_size, sizeof(header->text_len));
    used_index += sizeof(header->text_len);
    return used_index;
}

size_t api_ok_message_serialize(void *self, uint8_t *buffer) {
    ok_message_t *message = (ok_message_t *) self;
    size_t used_index = 0;
    used_index = message->header->serialize(message->header, buffer);
    memcpy(buffer + used_index, message->text, message->header->text_len);
    used_index += message->header->text_len;
    return used_index;
}

size_t api_not_ok_message_serialize(void *self, uint8_t *buffer) {
    not_ok_message_t *message = (not_ok_message_t *) self;
    size_t used_index = 0;
    used_index = message->header->serialize(message->header, buffer);
    uint32_t net_timestamp = htonl(message->timestamp);
    memcpy(buffer + used_index, &net_timestamp, sizeof(message->timestamp));
    used_index += sizeof(message->timestamp);
    memcpy(buffer + used_index, message->text, message->header->text_len);
    used_index += message->header->text_len;
    return used_index;
}

api_message_header_t *api_message_header_make(text_type_t text_type, uint16_t text_len) {
    api_message_header_t *header = (api_message_header_t *) malloc(sizeof(api_message_header_t));
    TERMINATE_IF(header == NULL, "failed to malloc when making api message header", ERR_MEM);
    header->text_type = text_type;
    header->text_len = text_len;
    header->serialize = api_header_serialize;
    header->destroy = api_message_header_destroy;
    header->refc = 0;
    return header;
}

api_message_t *base_message_make(size_t message_type_size, api_message_header_t *header) {
    api_message_t *message = (api_message_t *) malloc(message_type_size);
    TERMINATE_IF(message == NULL, "failed to malloc when making ok message", ERR_MEM);
    message->header = header;
    message->refc = 0;
    hold(header);
    return message;
}

ok_message_t *ok_message_make_with_header(api_message_header_t *header, uint8_t *text) {
    ok_message_t *message = (ok_message_t *) base_message_make(sizeof(ok_message_t), header);
    uint8_t *text_buf = (uint8_t *) malloc(sizeof(char) * header->text_len + 1);
    TERMINATE_IF(message == NULL, "failed to malloc for message text", ERR_MEM);
    memcpy(text_buf, text, header->text_len);
    text_buf[header->text_len] = 0;
    message->text = text_buf;
    message->serialize = api_ok_message_serialize;
    message->destroy = api_message_destroy;
    message->size = API_MSG_HDR_SIZE + header->text_len;
    return message;
}


not_ok_message_t *not_ok_message_make_with_header(api_message_header_t *header, uint32_t timestamp, uint8_t *text) {
    not_ok_message_t *message = (not_ok_message_t *) base_message_make(sizeof(not_ok_message_t), header);
    message->timestamp = timestamp;
    uint8_t *text_buf = (uint8_t *) malloc(sizeof(uint8_t) * header->text_len + 1);
    TERMINATE_IF(message == NULL, "failed to malloc for message text", ERR_MEM);
    memcpy(text_buf, text, header->text_len);
    text_buf[header->text_len] = 0;
    message->text = text_buf;
    message->destroy = api_message_destroy;
    message->serialize = api_not_ok_message_serialize;
    message->size = API_MSG_HDR_SIZE + sizeof(uint32_t) + header->text_len;
    return message;
}

ok_message_t *api_ok_message_make(uint16_t len, char *text) {
    api_message_header_t *header = api_message_header_make(API_MESSAGE_OK, len);
    header->text_len = len;
    return ok_message_make_with_header(header, (uint8_t *) text);
}

not_ok_message_t *api_not_ok_message_make(uint16_t len, const char *text, uint32_t timestamp) {
    api_message_header_t *header = api_message_header_make(API_MESSAGE_NOT_OK, len);
    header->text_len = len;
    return not_ok_message_make_with_header(header, timestamp, (uint8_t *) text);
}


api_message_header_t *api_message_header_deserialize(const uint8_t *bytes) {
    text_type_t text_type = bytes[0];
    uint16_t text_len = ntohs(*(uint16_t *) (bytes + 1));
    return api_message_header_make(text_type, text_len);
}

ok_message_t *api_ok_message_deserialize(api_message_header_t *header, uint8_t *bytes) {
    return ok_message_make_with_header(header, bytes);
}

not_ok_message_t *not_ok_message_deserialize(api_message_header_t *header, uint8_t *bytes) {
    uint32_t timestamp = ntohl(*(uint32_t *) bytes);
    return not_ok_message_make_with_header(header, timestamp, bytes + sizeof(uint32_t));
}



