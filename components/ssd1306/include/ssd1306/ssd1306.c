/**
 * \file
 *
 * \brief SSD1306 display controller driver.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "ssd1306.h"
#include "driver/i2c.h"

/**
 * \internal
 * \brief Initialize the hardware interface
 *
 * Initialize ESP's I2C interface
 */
static void ssd1306_interface_init(void)
{
    // This board doesn't use CS pin
    int i2c_master_port = SSD1306_I2C_PORT;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = SSD1306_SDA_PIN;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = SSD1306_SCL_PIN;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = SSD1306_I2C_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

/**
 * \brief Initialize the OLED controller
 *
 * Call this function to initialize the hardware interface and the OLED
 * controller. When initialization is done the display is turned on and ready
 * to receive data.
 */
void ssd1306_init(void)
{
	// Do a hard reset of the OLED display controller
	ssd1306_hard_reset();

	// Initialize the interface
	ssd1306_interface_init();

	// 1/32 Duty (0x0F~0x3F)
	ssd1306_write_command(SSD1306_CMD_SET_MULTIPLEX_RATIO);
	//ssd1306_write_command(0x1F);		//128 x 32
	ssd1306_write_command(0x3F);		//128 x 64

	// Shift Mapping RAM Counter (0x00~0x3F)
	ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_OFFSET);
	ssd1306_write_command(0x00);
	
	// Set memory adressing mode
	ssd1306_write_command(SSD1306_CMD_SET_MEMORY_ADDRESSING_MODE);
	ssd1306_write_command(0x00);
	
	// Set Mapping RAM Display Start Line (0x00~0x3F)
	ssd1306_write_command(SSD1306_CMD_SET_START_LINE(0x00));

	// Set Column Address 0 Mapped to SEG0
	ssd1306_write_command(SSD1306_CMD_SET_SEGMENT_RE_MAP_COL127_SEG0);

	// Set COM/Row Scan Scan from COM63 to 0
	ssd1306_write_command(SSD1306_CMD_SET_COM_OUTPUT_SCAN_DOWN);

	// Set COM Pins hardware configuration
	ssd1306_write_command(SSD1306_CMD_SET_COM_PINS);
	ssd1306_write_command(0x12);		//128 x 64
	//ssd1306_write_command(0x02);		//128 x 32

	ssd1306_set_contrast(0xFF);

	// Disable Entire display On
	ssd1306_write_command(SSD1306_CMD_ENTIRE_DISPLAY_AND_GDDRAM_ON);

	ssd1306_display_invert_disable();

	// Set Display Clock Divide Ratio / Oscillator Frequency (Default => 0x80)
	ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_CLOCK_DIVIDE_RATIO);
	//ssd1306_write_command(0x80);
	ssd1306_write_command(0xF0);

	// Enable charge pump regulator
	ssd1306_write_command(SSD1306_CMD_SET_CHARGE_PUMP_SETTING);
	ssd1306_write_command(0x14);

	// Set VCOMH Deselect Level
	ssd1306_write_command(SSD1306_CMD_SET_VCOMH_DESELECT_LEVEL);
	ssd1306_write_command(0x40); // Default => 0x20 (0.77*VCC)

	// Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	ssd1306_write_command(SSD1306_CMD_SET_PRE_CHARGE_PERIOD);
	ssd1306_write_command(0xF1);

//	ssd1306_display_on();
}
