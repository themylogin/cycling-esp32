#pragma once

#include <cstdint>
#include <cstring>

#include <esp_log.h>

template<size_t item_count, size_t item_size>
class ring_buffer
{
public:
    ring_buffer()
    {
        this->index = 0;
        this->count = 0;
    }

    void write(uint8_t* src, size_t length)
    {
        if (this->count == item_count)
        {
            ESP_LOGW(TAG, "Ring buffer overflow");

            memcpy(this->data[this->index], src, length);
            this->lengths[index] = length;

            this->index = (this->index + 1) % item_count;
        }
        else
        {
            size_t i = (this->index + this->count) % item_count;
            memcpy(this->data[i], src, length);
            this->lengths[i] = length;

            this->count++;
        }
    }

    bool read(uint8_t* dst, size_t* length)
    {
        if (this->count == 0)
        {
            return false;
        }

        memcpy(dst, this->data[this->index], this->lengths[this->index]);
        *length = this->lengths[this->index];

        this->index = (this->index + 1) % item_count;
        this->count--;

        return true;
    }

private:
    static constexpr const char* TAG = "ring_buffer";

    uint8_t data[item_count][item_size];
    size_t lengths[item_count];

    size_t index;
    size_t count;
};
