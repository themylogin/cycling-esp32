#pragma once

#include "lora/Receiver.hpp"
#include "lora/Sender.hpp"

using lora::Receiver;
using lora::Sender;
using lora::max_outgoing_packet_size;

typedef struct {
    Receiver* receiver;
    Sender* sender;
    uint8_t buffer[max_outgoing_packet_size];
} lora_context;

void lora_init(lora_context* context);

void lora_process(lora_context* context);

void lora_task(void* pvParameters);
