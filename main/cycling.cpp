#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

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

    // xTaskCreate(vATaskFunction, "PrintTask", 2048, NULL, 0, NULL);
        
    return 0;
}
