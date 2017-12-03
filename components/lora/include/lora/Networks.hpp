#pragma once

#include <map>
#include <set>
#include <vector>

#include <esp_log.h>

#include "lora/NeighborsListProcessor.hpp"
#include "lora/Network.hpp"
#include "lora/Sender.hpp"

using std::map;
using std::vector;

namespace lora
{

typedef struct
{
    network_t network;
    address_t address;
} SetNetworksRequestNetwork;

class Networks
{
public:
    Networks(Sender* sender, NeighborsListProcessor* neighbors_list_processor)
    {
        this->sender = sender;
        this->neighbors_list_processor = neighbors_list_processor;
    }

    Network* get(network_t id)
    {
        auto network = this->networks.find(id);
        if (network != this->networks.end())
        {
            return (*network).second;
        }

        return nullptr;
    }

    void set(vector<SetNetworksRequestNetwork> networks)
    {
        std::set<network_t> leave;
        for (auto& it : this->networks)
        {
            leave.insert(it.first);
        }

        for (auto& request : networks)
        {
            leave.erase(request.network);

            auto it = this->networks.find(request.network);
            if (it != this->networks.end())
            {
                auto network = (*it).second;
                if (network->get_address() == request.address)
                {
                    continue;
                }

                ESP_LOGI(TAG, "Leaving network %x because of address change", request.network);
                delete network;                
            }

            ESP_LOGI(TAG, "Joining network %x with address %x", request.network, request.address);
            this->networks[request.network] = new Network(request.network, request.address, this->sender,
                                                          this->neighbors_list_processor);
        }

        for (auto network : leave)
        {
            ESP_LOGI(TAG, "Leaving network %x", network);
            auto it = this->networks.find(network);
            this->networks.erase(it);
            delete (*it).second;
        }
    }

private:
    static constexpr const char* TAG = "lora::networks";

    Sender* sender;
    NeighborsListProcessor* neighbors_list_processor;

    map<network_t, Network*> networks;
};

};
