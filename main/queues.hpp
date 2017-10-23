#include "ring_buffer.hpp"
#include "radio_message.hpp"

extern RingBuffer<RadioMessage> incoming;
extern RingBuffer<RadioMessage> outgoing;

void queues_init();
