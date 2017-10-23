#include "bluetooth.hpp"

#include <inttypes.h>
#include <stdint.h>

#include "btstack.h"

#include "queues.hpp"

#define RFCOMM_SERVER_CHANNEL 1

static btstack_packet_callback_registration_t hci_event_callback_registration;

static uint8_t spp_service_buffer[150];

static uint16_t rfcomm_cid = 0;

void bluetooth_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size)
{
    UNUSED(channel);

    switch (packet_type) {
        case HCI_EVENT_PACKET:
            switch (hci_event_packet_get_type(packet))
            {
                case HCI_EVENT_PIN_CODE_REQUEST:
                    {
                        bd_addr_t event_addr;
                        printf("Pin code request - using '0000'\n");
                        hci_event_pin_code_request_get_bd_addr(packet, event_addr);
                        gap_pin_code_response(event_addr, "0000");
                    }
                    break;

                case HCI_EVENT_USER_CONFIRMATION_REQUEST:
                    printf("SSP User Confirmation Request with numeric value '%06" PRIu32 "'\n", little_endian_read_32(packet, 8));
                    printf("SSP User Confirmation Auto accept\n");
                    break;

                case RFCOMM_EVENT_INCOMING_CONNECTION:
                    {
                        bd_addr_t event_addr;
                        rfcomm_event_incoming_connection_get_bd_addr(packet, event_addr); 
                        uint8_t rfcomm_channel_nr = rfcomm_event_incoming_connection_get_server_channel(packet);
                        rfcomm_cid = rfcomm_event_incoming_connection_get_rfcomm_cid(packet);
                        printf("RFCOMM channel %u requested for %s\n", rfcomm_channel_nr, bd_addr_to_str(event_addr));
                        rfcomm_accept_connection(rfcomm_cid);
                    }
                    break;
                    
                case RFCOMM_EVENT_CHANNEL_OPENED:
                    if (rfcomm_event_channel_opened_get_status(packet))
                    {
                        printf("RFCOMM channel open failed, status %u\n", rfcomm_event_channel_opened_get_status(packet));
                    }
                    else
                    {
                        rfcomm_cid = rfcomm_event_channel_opened_get_rfcomm_cid(packet);
                        uint16_t rfcomm_mtu = rfcomm_event_channel_opened_get_max_frame_size(packet);
                        printf("RFCOMM channel open succeeded. New RFCOMM Channel ID %u, max frame size %u\n", rfcomm_cid, rfcomm_mtu);

                        bluetooth_send_messages();
                    }
                    break;

                case RFCOMM_EVENT_CAN_SEND_NOW:
                    {
                        if (incoming.canStartConsuming())
                        {
                            RadioMessage message = incoming.startConsuming();
                            rfcomm_send(rfcomm_cid, message.buffer, message.size);
                            incoming.endConsuming(message);

                            bluetooth_send_messages();
                        }
                    }
                    break;

                case RFCOMM_EVENT_CHANNEL_CLOSED:
                    printf("RFCOMM channel closed\n");
                    rfcomm_cid = 0;
                    break;

                default:
                    break;
            }
            break;
                        
        case RFCOMM_DATA_PACKET:
            {
                RadioMessage message = outgoing.startProducing();
                message.size = size;
                memcpy(message.buffer, packet, size);
                outgoing.endProducing(message);

                RadioMessage copy = incoming.startProducing();
                copy.size = size;
                memcpy(copy.buffer, packet, size);
                incoming.endProducing(copy);
                bluetooth_send_messages();
            }
            break;

        default:
            break;
    }
}

void bluetooth_init()
{
    hci_event_callback_registration.callback = &bluetooth_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    l2cap_init();

    rfcomm_init();
    rfcomm_register_service(bluetooth_packet_handler, RFCOMM_SERVER_CHANNEL, 0xffff);

    sdp_init();
    memset(spp_service_buffer, 0, sizeof(spp_service_buffer));
    spp_create_sdp_record(spp_service_buffer, 0x10001, RFCOMM_SERVER_CHANNEL, "SPP Streamer");
    sdp_register_service(spp_service_buffer);

    gap_set_class_of_device(020500);

    gap_ssp_set_io_capability(SSP_IO_CAPABILITY_DISPLAY_YES_NO);
    gap_set_local_name("VPU 00:00:00:00:00:00");
    gap_discoverable_control(1);

    hci_power_control(HCI_POWER_ON);
}

void bluetooth_send_messages()
{
    rfcomm_request_can_send_now_event(rfcomm_cid);
}
