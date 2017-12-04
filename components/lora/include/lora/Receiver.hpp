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
        if (size <= packet_header_size)
        {
            ESP_LOGW(TAG, "Received packet too small (%d)", size);
            return;
        }

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

        uint8_t bt_packet[256];
        size_t bt_packet_size = 0;

        bt_packet[bt_packet_size] = 0;
        bt_packet_size += 1;

        *reinterpret_cast<network_t*>(bt_packet + 1) = packet->network;
        bt_packet_size += sizeof(network_t);

        memcpy(bt_packet + bt_packet_size, packet->payload, packet_payload_size(packet, size));
        bt_packet_size += packet_payload_size(packet, size);

        this->bt_sender->send(bt_packet, bt_packet_size);
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
