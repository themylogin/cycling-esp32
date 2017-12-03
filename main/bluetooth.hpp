#pragma once

#include "bluetooth/Receiver.hpp"
#include "bluetooth/Sender.hpp"

extern bluetooth::Sender* bluetooth_sender;
extern bluetooth::Receiver* bluetooth_receiver;

void bluetooth_init();

void bluetooth_send_messages();
