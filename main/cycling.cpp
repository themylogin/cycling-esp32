#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "lora/NeighborsListProcessor.hpp"
#include "lora/Networks.hpp"
#include "lora/Receiver.hpp"
#include "lora/Sender.hpp"

#include "ssd1306/gfx_mono.h"
#include "bluetooth.hpp"

/*
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void vATaskFunction( void *pvParameters )
{
    for( ;; )
    {
        if (b.canStartConsuming())
        {
            RadioMessage dst = b.startConsuming();
            printf("===\n");
            for (int i = 0; i < dst.size; i++)
            {
                printf("%c", dst.buffer[i]);
            }
            printf("\n");
            b.endConsuming(dst);
        }
        taskYIELD();
    }

    vTaskDelete( NULL );
}
*/

extern "C"
int btstack_main(int argc, const char* argv[])
{
    (void) (argc);
    (void) (argv);

    lora::Sender* lora_sender = new lora::Sender;

    lora::NeighborsListProcessor* neighbots_list_processor = new lora::NeighborsListProcessor;
    lora::Networks* networks = new lora::Networks(lora_sender, neighbots_list_processor);

    bluetooth_receiver = new bluetooth::Receiver(networks);
    bluetooth_sender = new bluetooth::Sender(bluetooth_send_messages);

    lora::Receiver* lora_receiver = new lora::Receiver(networks, bluetooth_sender);

    bluetooth_init();
    
    gfx_mono_init();
    ssd1306_display_on();
    gfx_mono_draw_string("Hello world!", 0,	0, &sysfont);
    gfx_mono_draw_line(0, 18, 96, 18, GFX_PIXEL_SET);
    gfx_mono_put_framebuffer();    

    // xTaskCreate(vATaskFunction, "PrintTask", 2048, NULL, 0, NULL);
        
    return 0;
}
