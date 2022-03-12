# BCIT Datacomm lecture on layered applications

A sample application that reads and writes from a UNIX file descriptor. In this case, the implementation is done on a
file descriptor, but the concepts are also applicable to network sockets.

## Features

- Layered communication stack
- Factory pattern
- Chain of Command pattern
- Observer pattern

## Objective

Demonstrate the implementation of layered software that is applicable to any IPC. Also, demonstrate how to build a
communication stack from the bottom up. Finally, introduce students to OOP patterns in C.

## Protocols

### Layer 1

    <L1_message> := <header> <data>
    <hedader> := <data_len> <reserved>
    <reserved> := uint32_t
    <data_len> := uint16_t

### Layer 2

    <L2_message> := <cipher_message> | <clear_message>
    <cipher_message> := <cipher_key>  <cipher_func(<clear_message>, <cipher_key> )>
    <clear_message> := <decipher_func(<cipher_message>, <cipher_key> := uint8_t)>
    <cipher_key> := uint8_t
    <cipher_func> := for byte in <clear_message> -> byte + <cipher_key>
    <decipher_func> := for byte in <clear_message> -> byte - <cipher_key>

### Layer 3

    <L3_message> := <ok_message> | <not_ok_message>
    <message> := <message_header> <other_message_content> <text>
    <message_header> := <text_type> <text_len>
    <other_message_content> := void | <not_ok_message_content>
    <not_ok_message_content> := <timestamp>

    <text_type> := uint8_t  
    <text_type> :=  
          <ok> := 0 
        | <not_ok> := 1
    <text_len> := uint16_t;
    <text> := uint8_t | uint8_t <text>
    <timestamp> := uint32_t

## Requirements

- All stack layers must construct correctly and may not proceed until this is asserted

