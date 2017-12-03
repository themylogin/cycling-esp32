#pragma once

#include "lora/Neighbors.hpp"

namespace lora
{

class Network;

class NeighborsListProcessor
{
public:
    void receive(uint8_t* data, size_t size, Neighbors* neighbors);

    void send(Neighbors* neighbors, Network* network, time_t time);

private:
    static constexpr const char* TAG = "lora::neighbors_list_processor";
};

};
