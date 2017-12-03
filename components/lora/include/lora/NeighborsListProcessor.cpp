#include "lora/NeighborsListProcessor.hpp"

#include <vector>

#include <esp_log.h>

#include "lora/Neighbors.hpp"
#include "lora/Network.hpp"
#include "lora/Packet.hpp"

using std::vector;

namespace lora
{

void NeighborsListProcessor::receive(uint8_t* data, size_t size, Neighbors* neighbors)
{
    auto packet = reinterpret_cast<Packet*>(data);

    if (packet->flags & packet_flags::neighbors_list)
    {
        auto count = packet_payload_size(packet, size) / sizeof(address_t);
        auto raw_addresses = reinterpret_cast<address_t*>(packet->payload);

        vector<address_t> addresses(count);
        std::copy(raw_addresses, raw_addresses + count, addresses.begin());

        ESP_LOGI(TAG, "Received %d neighbors from %x", addresses.size(), packet->src);
        neighbors->update_neighbor_neighbors(packet->src, addresses);
    }
}

void NeighborsListProcessor::send(Neighbors* neighbors, Network* network, time_t time)
{
    auto addresses = neighbors->get_neighbors(time);

    ESP_LOGI(TAG, "Broadcasting %d neighbors", addresses.size());
    network->send(reinterpret_cast<uint8_t*>(addresses.data()),
                  addresses.size() * sizeof(address_t),
                  packet_flags::neighbors_list);
}

};
