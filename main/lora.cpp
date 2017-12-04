#include "lora.hpp"

#include <sys/time.h>

#include <esp_log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "LoRa.h"
#include "SPI.h"

static const char* TAG = "lora";

void lora_init(lora_context* context)
{    
    SPI.begin(5, 19, 27, 18);
    LoRa.setPins(18, 14, 26);

    if (!LoRa.begin(433E6))
    {
        ESP_LOGE(TAG, "LoRa.begin failed");
    }
}

void lora_process(lora_context* context)
{
    if (LoRa.parsePacket())
    {
        size_t size = 0;
        while (LoRa.available())
        {
            context->buffer[size++] = LoRa.read();
        }

        ESP_LOGI(TAG, "Received packet");
        context->receiver->receive(context->buffer, size, LoRa.packetRssi(), time(NULL));
    }

    size_t size;
    if (context->sender->request_data(context->buffer, &size))
    {
        ESP_LOGI(TAG, "Sending packet");
        LoRa.beginPacket();
        LoRa.write(context->buffer, size);
        LoRa.endPacket();
    }
}

void lora_task(void* pvParameters)
{
    lora_context* context = reinterpret_cast<lora_context*>(pvParameters);

    while (true)
    {
        lora_process(context);
        taskYIELD();
    }
}
