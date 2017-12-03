#pragma once

#include <esp_log.h>

#include "bluetooth/Sender.hpp"
#include "lora/Networks.hpp"
#include "lora/Packet.hpp"

namespace lora
{

class Receiver
{
public:
    Receiver(Networks* networks, bluetooth::Sender* bt_sender)
    {
        this->networks = networks;
        this->bt_sender = bt_sender;
    }

    void receive(uint8_t* data, size_t size, uint8_t rssi, time_t time)
    {
        auto packet = reinterpret_cast<Packet*>(data);

        if (packet->network != 0)
        {
            auto network = this->networks->get(packet->network);
            if (network == nullptr)
            {
                ESP_LOGI(TAG, "Received packet for invalid network %x", packet->network);
                return;
            }
            if (!network->receive(data, size, rssi, time))
            {
                return;
            }
        }

        if (this->is_service_packet(packet))
        {
            return;
        }

        this->bt_sender->send(packet->payload, packet_payload_size(packet, size));
    }

private:
    static constexpr const char* TAG = "lora::network";

    Networks* networks;
    bluetooth::Sender* bt_sender;

    bool is_service_packet(Packet* packet)
    {
        return packet->flags & (packet_flags::neighbors_list);
    }
};

};
