#ifndef CONF_SSD1306_H_INCLUDED
#define CONF_SSD1306_H_INCLUDED

#  define SSD1306_SDA_PIN       GPIO_NUM_4
#  define SSD1306_SCL_PIN       GPIO_NUM_15
#  define SSD1306_RES_PIN       GPIO_NUM_16
#  define SSD1306_I2C_PORT      I2C_NUM_1
#  define SSD1306_I2C_ADDR      0x3c

// hc595 is limited 5 Mhz @ 2V
// ssd1306 is limited 10 Mhz @ 3.3V
#  define SSD1306_I2C_FREQ_HZ  5000000

// Board independent configuration

// Minimum clock period is 50ns@3.3V -> max frequency is 20MHz
#define SSD1306_CLOCK_SPEED          UG_2832HSWEG04_BAUDRATE
#define SSD1306_DISPLAY_CONTRAST_MAX 40
#define SSD1306_DISPLAY_CONTRAST_MIN 20

#endif /* CONF_SSD1306_H_INCLUDED */
