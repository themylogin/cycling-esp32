#pragma once

#include <esp_log.h>
#include <esp_system.h>

#include "lora/Deduplicator.hpp"
#include "lora/Neighbors.hpp"
#include "lora/NeighborsListProcessor.hpp"
#include "lora/Packet.hpp"
#include "lora/Sender.hpp"
#include "lora/types.hpp"

using std::map;
using std::tuple;

namespace lora
{

const int DEDUPLICATOR_SIZE = 256;

class Network
{
public:
    Network(network_t network, address_t address, Sender* sender, NeighborsListProcessor* neighbors_list_processor)
    {
        this->network = network;
        this->address = address;

        this->sender = sender;
        this->neighbors_list_processor = neighbors_list_processor;

        this->deduplicator = new Deduplicator<DEDUPLICATOR_SIZE>;

        this->neighbors = new Neighbors(this->address);
    }

    ~Network()
    {
        delete this->neighbors;

        delete this->deduplicator;
    }

    address_t get_address()
    {
        return this->address;
    }

    bool receive(uint8_t* data, size_t size, uint8_t rssi, time_t time)
    {
        auto packet = reinterpret_cast<Packet*>(data);

        if (packet->src == this->address)
        {
            return false;
        }

        if (!this->deduplicator->receive(packet))
        {
            return false;
        }

        if (!packet->forwarder)
        {
            this->neighbors->update_neighbor(packet->src, rssi, time);
        }

        this->neighbors_list_processor->receive(data, size, this->neighbors);

        if (!this->neighbors->every_neighbor_can_hear(packet->src, time))
        {
            if (!packet->forwarder || !this->neighbors->every_neighbor_can_hear(packet->forwarder, time))
            {
                ESP_LOGI(TAG, "Forwarding packet with src=%x, forwarder=%x",
                         packet->src, packet->forwarder);
                this->raw_send(packet->src, this->address, packet->seq, packet->flags,
                               packet->payload, packet_payload_size(packet, size));
            }
        }

        return true;
    }

    void send(uint8_t* payload, size_t size, uint32_t flags = 0)
    {
        this->raw_send(this->address, 0, esp_random(), flags, payload, size);
    }

    void periodic(time_t time)
    {
        this->neighbors_list_processor->send(this->neighbors, this, time);
    }

private:
    static constexpr const char* TAG = "lora::network";

    network_t network;
    address_t address;
    
    Sender* sender;
    NeighborsListProcessor* neighbors_list_processor;

    Deduplicator<DEDUPLICATOR_SIZE>* deduplicator;

    Neighbors* neighbors;

    void raw_send(address_t src, address_t forwarder, uint32_t seq, uint32_t flags, uint8_t* payload, size_t size)
    {
        Packet packet;
        packet.network = this->network;
        packet.src = src;
        packet.forwarder = forwarder;
        packet.seq = seq;
        packet.flags = flags;
        memcpy(&packet.payload, payload, size);

        this->sender->send(reinterpret_cast<uint8_t*>(&packet), packet_header_size + size);
    }
};

};
