#include "queues.hpp"

RingBuffer<RadioMessage> incoming;
RingBuffer<RadioMessage> outgoing;

void queues_init()
{    
    for (int i = 0; i < 10; i++)
    {
        RadioMessage incoming_message;
        incoming_message.buffer = new uint8_t[256];
        incoming_message.size = 0;
        incoming.add(incoming_message);

        RadioMessage outgoing_message;
        outgoing_message.buffer = new uint8_t[256];
        outgoing_message.size = 0;
        outgoing.add(outgoing_message);
    }
}
