#pragma once

#include <algorithm>
#include <vector>

#include <esp_log.h>

#include "lora/Networks.hpp"
#include "lora/types.hpp"

using std::vector;

using lora::Networks;
using lora::SetNetworksRequestNetwork;

namespace bluetooth
{

namespace command
{
    enum command : uint8_t
    {
        set_networks = 0x00,
        send_packet = 0x01,
    };
};

class Receiver
{
public:
    Receiver(Networks* networks)
    {
        this->networks = networks;
    }

    void receive(uint8_t* data, size_t size)
    {
        if (size == 0)
        {
            ESP_LOGE(TAG, "Received zero-size packet");
            return;
        }

        uint8_t command = data[0];

        data++;
        size--;

        switch (command)
        {
            case command::set_networks:
            {
                ESP_LOGI(TAG, "Received `set_networks` command");

                auto raw_networks = reinterpret_cast<SetNetworksRequestNetwork*>(data);
                size_t count = size / sizeof(SetNetworksRequestNetwork);

                vector<SetNetworksRequestNetwork> networks(count);
                std::copy(raw_networks, raw_networks + count, networks.begin());

                ESP_LOGI(TAG, "Joining %d networks", networks.size());
                this->networks->set(networks);
                
                break;
            }

            case command::send_packet:
            {
                auto network_id = *reinterpret_cast<lora::network_t*>(data);
                auto network = this->networks->get(network_id);
                if (network == nullptr)
                {
                    ESP_LOGE(TAG, "Requested sending packet to unknown network %d", network_id);
                    return;
                }
                
                data += sizeof(lora::network_t);
                size -= sizeof(lora::network_t);

                ESP_LOGI(TAG, "Sending packet of size %d to network %x", size, network_id);
                network->send(data, size);

                break;
            }

            default:
            {
                ESP_LOGE(TAG, "Received Unknown command %x", command);

                break;
            }
        }
    }

private:
    static constexpr const char* TAG = "bluetooth::receiver";

    Networks* networks;
};

};
