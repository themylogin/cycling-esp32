#pragma once

#include <algorithm>
#include <map>
#include <set>
#include <vector>

#include <esp_log.h>

#include "lora/types.hpp"

using std::map;
using std::set;
using std::vector;

namespace lora
{

typedef struct {
    uint8_t rssi;
    time_t time;
    vector<address_t> neighbors;
} Neighbor;

class Neighbors
{
public:
    Neighbors(address_t address)
    {
        this->address = address;
    }

    void update_neighbor(address_t src, uint8_t rssi, time_t time)
    {
        auto& record = this->neighbors[src];

        record.rssi = rssi;
        record.time = time;
    }

    void update_neighbor_neighbors(address_t src, vector<address_t> neighbors)
    {
        auto& record = this->neighbors[src];

        record.neighbors.clear();
        std::copy_if(neighbors.begin(), neighbors.end(), std::back_inserter(record.neighbors),
                     [this](address_t neighbor) { return neighbor != this->address; });
    }

    vector<address_t> get_neighbors(time_t time)
    {
        vector<address_t> result;
        for (auto it = this->neighbors.begin(); it != this->neighbors.end(); it++)
        {
            auto& neighbor = (*it).second;

            if (this->is_active(neighbor, time))
            {
                result.push_back((*it).first);
            }
        }

        return result;
    }

    bool every_neighbor_can_hear(address_t src, time_t time)
    {
        set<address_t> known_devices;
        for (auto it = this->neighbors.begin(); it != this->neighbors.end(); it++)
        {
            auto address = (*it).first;
            auto& neighbor = (*it).second;

            known_devices.insert(address);
            std::copy(neighbor.neighbors.begin(), neighbor.neighbors.end(),
                      std::inserter(known_devices, known_devices.begin()));
        }

        auto cant_hear = known_devices;
        cant_hear.erase(src);
        auto linked_to_it = this->neighbors.find(src);
        if (linked_to_it != this->neighbors.end())
        {
            for (auto& linked_to : (*linked_to_it).second.neighbors)
            {
                cant_hear.erase(linked_to);
            }
        }
        else
        {
            for (auto it = this->neighbors.begin(); it != this->neighbors.end(); it++)
            {
                auto address = (*it).first;
                auto& neighbor = (*it).second;

                if (this->is_active(neighbor, time))
                {
                    if (std::find(neighbor.neighbors.begin(), neighbor.neighbors.end(), src) !=
                        neighbor.neighbors.end())
                    {
                        cant_hear.erase(address);
                    }
                }
            }
        }
        if (cant_hear.size() == 0)
        {
            ESP_LOGI(TAG, "Every neighbor can hear %x because everyone has him in neighbor list", src);
            return true;
        }

        set<address_t> forwarders;
        for (auto it = this->neighbors.begin(); it != this->neighbors.end(); it++)
        {
            auto address = (*it).first;
            auto& neighbor = (*it).second;

            if (this->is_active(neighbor, time))
            {
                if (address < this->address)
                {
                    forwarders.insert(address);
                }
            }
        }
        forwarders.erase(src);

        set<address_t> should_be_linked_to = cant_hear;
        should_be_linked_to.insert(src);

        for (auto& forwarder : forwarders)
        {
            bool linked_to_everyone = true;
            for (auto& address : should_be_linked_to)
            {
                if (std::find(this->neighbors[forwarder].neighbors.begin(),
                              this->neighbors[forwarder].neighbors.end(),
                              address) ==
                    this->neighbors[forwarder].neighbors.end())
                {
                    linked_to_everyone = false;
                    break;
                }
            }
            if (linked_to_everyone)
            {
                ESP_LOGI(TAG, "Every neighbor can hear %x because %x will forward packets for him",
                         src, forwarder);
                return true;
            }
        }

        return false;
    }

private:
    bool is_active(const Neighbor& neighbor, time_t time)
    {
        return (
            neighbor.rssi <= this->rssi_threshold &&
            neighbor.time >= time - this->timeout
        );
    }

private:
    static constexpr const char* TAG = "lora::neighbors";

    static const uint8_t rssi_threshold = 128;
    static const int timeout = 15;

    address_t address;
    map<address_t, Neighbor> neighbors;
};

};
