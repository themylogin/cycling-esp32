#pragma once

#include <esp_log.h>

#include "ring_buffer.hpp"

namespace bluetooth
{

template<size_t item_count, size_t item_size>
class _Sender
{
public:
    _Sender(void(*request_transmission_callback)())
    {
        this->buffer = new ring_buffer<item_count, item_size>;
        this->request_transmission_callback = request_transmission_callback;
    }

    void send(uint8_t* data, size_t size)
    {
        ESP_LOGI(TAG, "Sending %d bytes", size);

        this->buffer->write(data, size);
        this->request_transmission_callback();
    }

    bool request_data(uint8_t* data, size_t* size)
    {
        return this->buffer->read(data, size);
    }

private:
    static constexpr const char* TAG = "bluetooth::sender";

    ring_buffer<item_count, item_size>* buffer;
    void(*request_transmission_callback)();
};

const size_t outgoing_packet_queue_size = 10;
const size_t max_outgoing_packet_size = 256;

typedef _Sender<outgoing_packet_queue_size, max_outgoing_packet_size> Sender;

};
