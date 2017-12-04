#include "Arduino.h"

#include "freertos/task.h"

void delay(int ms)
{
    vTaskDelay(ms * configTICK_RATE_HZ / 1000);
}
