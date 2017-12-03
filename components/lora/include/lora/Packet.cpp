#include "Packet.hpp"

namespace lora
{

size_t packet_payload_size(Packet* packet, size_t size)
{
    return size - packet_header_size;
}

};
