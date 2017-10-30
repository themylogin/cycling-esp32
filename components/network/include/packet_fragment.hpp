#include <stdint.h>

const size_t packet_fragment_max_size = 236;

typedef struct {
    uint32_t network;
    uint32_t src;
    union {
        uint64_t seq;
        struct {
            uint16_t fragment;
            uint16_t nr_fragments;
            uint32_t seq;
        } fragment;
    } seq;
    uint32_t reserved;
    uint8_t data[packet_fragment_max_size];

    size_t size;
} packet_fragment;

/*
static_assert(sizeof(packet_fragment) == 256 + 1,
              "Packet fragment size should be 256");
*/
