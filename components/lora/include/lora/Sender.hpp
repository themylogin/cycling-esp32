#pragma once

#include "ring_buffer.hpp"

namespace lora
{

template<size_t item_count, size_t item_size>
class _Sender
{
public:
    _Sender()
    {
        this->buffer = new ring_buffer<item_count, item_size>;
    }

    void send(uint8_t* data, size_t size)
    {
        this->buffer->write(data, size);
    }

    bool request_data(uint8_t* data, size_t* size)
    {
        return this->buffer->read(data, size);
    }

private:
    ring_buffer<item_count, item_size>* buffer;
};

const size_t outgoing_packet_queue_size = 10;
const size_t max_outgoing_packet_size = 255;

typedef _Sender<outgoing_packet_queue_size, max_outgoing_packet_size> Sender;

};
