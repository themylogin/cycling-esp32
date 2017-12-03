#pragma once

#include <cstdlib>
#include <cstring>
#include <map>
#include <utility>

#include <esp_log.h>

#include "lora/Packet.hpp"

using std::make_tuple;
using std::tuple;

namespace lora
{

template<size_t size>
class Deduplicator
{
public:
    Deduplicator()
    {
        memset(this->seqs, 0, sizeof(this->seqs));
        this->index = 0;
    }

    bool receive(Packet* packet)
    {
        auto seq = make_tuple<>(packet->src, packet->seq);
        for (size_t i = 0; i < size; i++)
        {
            if (this->seqs[i] == seq)
            {
                ESP_LOGI(TAG, "Received duplicate packet from %x with seq=%x", packet->src, packet->seq);
                return false;
            }
        }

        this->seqs[this->index] = seq;
        this->index = (this->index + 1) % size;

        return true;
    }

private:
    static constexpr const char* TAG = "lora::deduplicator";

    tuple<address_t, uint32_t> seqs[size];
    size_t index;
};

};
