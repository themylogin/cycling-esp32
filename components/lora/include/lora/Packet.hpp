#pragma once

#include <cstdint>
#include <cstring>

#include "lora/types.hpp"

namespace lora
{

typedef struct {
    network_t network;
    address_t src;
    address_t forwarder;
    uint32_t seq;
    uint32_t flags;

    uint8_t payload[232];
} Packet;

const size_t packet_header_size = sizeof(Packet) - sizeof(Packet::payload);

size_t packet_payload_size(Packet* packet, size_t size);

static_assert(sizeof(Packet) == 252, "sizeof(Packet) == 252");

namespace packet_flags
{
    enum packet_flags : uint32_t
    {
        neighbors_list = 0x01,
    };
};

};
