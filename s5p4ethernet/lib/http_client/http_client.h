#ifndef __S5_PROJECT_HTTP_CLIENT_H
#define __S5_PROJECT_HTTP_CLIENT_H

#include <mbed.h>
#include <rtos.h>
#include "EthernetInterface.h"
#include "Logger.h"

#define DEBUG_HTTP_CLIENT "HTTP CLIENT"

typedef struct {
  char message[100];
} http_message_t;

extern Mail<http_message_t, 10> http_messages;

void start_http_sender(Thread *);

http_message_t create_http_message(uint16_t, uint16_t, uint16_t, char, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

#endif
