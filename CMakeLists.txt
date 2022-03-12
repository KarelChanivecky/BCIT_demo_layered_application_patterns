cmake_minimum_required(VERSION 3.16)
project(layers C)

set(CMAKE_C_STANDARD 11)
add_compile_options(-fms-extensions)

add_executable(
        layers
        main.c
        layer_definitions.h
        api.c
        api.h
        common.h
        buffer_layer.c buffer_layer.h fd_wraper.c fd_wraper.h cipher_layer.c cipher_layer.h api_message_layer.c api_message_layer.h factory.h factory.c dlinked_list.c dlinked_list.h layer_definitions.c event_loop.c event_loop.h api_message.h api_message.c common.c)
