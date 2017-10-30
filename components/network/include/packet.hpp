#include <stdint.h>

const size_t packet_max_fragments = 18;
const size_t packet_max_size = packet_max_fragments * packet_fragment_max_size;

typedef struct {
    uint32_t network;
    uint32_t src;
    uint64_t seq;

    uint8_t data[packet_max_size];
    size_t size;
} packet;
