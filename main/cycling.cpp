#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "ssd1306/gfx_mono.h"
#include "bluetooth.hpp"
#include "queues.hpp"

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

    queues_init();
    bluetooth_init();
    gfx_mono_init();
    ssd1306_display_on();
    gfx_mono_draw_string("Hello world!", 0,	0, &sysfont);
    gfx_mono_draw_line(0, 18, 96, 18, GFX_PIXEL_SET);
    gfx_mono_put_framebuffer();    

    // xTaskCreate(vATaskFunction, "PrintTask", 2048, NULL, 0, NULL);
        
    return 0;
}
