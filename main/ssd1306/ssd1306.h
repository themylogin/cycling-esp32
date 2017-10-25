#ifndef SSD1306_H_INCLUDED
#define SSD1306_H_INCLUDED

// controller and OLED configuration file
#include "conf_ssd1306.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

//! \name Fundamental Command defines
//@{
#define SSD1306_CMD_SET_LOW_COL(column)             (0x00 | (column))
#define SSD1306_CMD_SET_HIGH_COL(column)            (0x10 | (column))
#define SSD1306_CMD_SET_MEMORY_ADDRESSING_MODE      0x20
#define SSD1306_CMD_SET_COLUMN_ADDRESS              0x21
#define SSD1306_CMD_SET_PAGE_ADDRESS                0x22
#define SSD1306_CMD_SET_START_LINE(line)            (0x40 | (line))
#define SSD1306_CMD_SET_CONTRAST_CONTROL_FOR_BANK0  0x81
#define SSD1306_CMD_SET_CHARGE_PUMP_SETTING         0x8D
#define SSD1306_CMD_SET_SEGMENT_RE_MAP_COL0_SEG0    0xA0
#define SSD1306_CMD_SET_SEGMENT_RE_MAP_COL127_SEG0  0xA1
#define SSD1306_CMD_ENTIRE_DISPLAY_AND_GDDRAM_ON    0xA4
#define SSD1306_CMD_ENTIRE_DISPLAY_ON               0xA5
#define SSD1306_CMD_SET_NORMAL_DISPLAY              0xA6
#define SSD1306_CMD_SET_INVERSE_DISPLAY             0xA7
#define SSD1306_CMD_SET_MULTIPLEX_RATIO             0xA8
#define SSD1306_CMD_SET_DISPLAY_ON                  0xAF
#define SSD1306_CMD_SET_DISPLAY_OFF                 0xAE
#define SSD1306_CMD_SET_PAGE_START_ADDRESS(page)    (0xB0 | (page & 0x07))
#define SSD1306_CMD_SET_COM_OUTPUT_SCAN_UP          0xC0
#define SSD1306_CMD_SET_COM_OUTPUT_SCAN_DOWN        0xC8
#define SSD1306_CMD_SET_DISPLAY_OFFSET              0xD3
#define SSD1306_CMD_SET_DISPLAY_CLOCK_DIVIDE_RATIO  0xD5
#define SSD1306_CMD_SET_PRE_CHARGE_PERIOD           0xD9
#define SSD1306_CMD_SET_COM_PINS                    0xDA
#define SSD1306_CMD_SET_VCOMH_DESELECT_LEVEL        0xDB
#define SSD1306_CMD_NOP                             0xE3
//@}
//! \name Graphic Acceleration Command defines
//@{
#define SSD1306_CMD_SCROLL_H_RIGHT                  0x26
#define SSD1306_CMD_SCROLL_H_LEFT                   0x27
#define SSD1306_CMD_CONTINUOUS_SCROLL_V_AND_H_RIGHT 0x29
#define SSD1306_CMD_CONTINUOUS_SCROLL_V_AND_H_LEFT  0x2A
#define SSD1306_CMD_DEACTIVATE_SCROLL               0x2E
#define SSD1306_CMD_ACTIVATE_SCROLL                 0x2F
#define SSD1306_CMD_SET_VERTICAL_SCROLL_AREA        0xA3
//@}

// Atmel ASF to ESP-IDF:
#define arch_ioport_set_pin_level    gpio_set_level
    
#define ssd1306_reset_clear()    arch_ioport_set_pin_level(SSD1306_RES_PIN, false)
#define ssd1306_reset_set()      arch_ioport_set_pin_level(SSD1306_RES_PIN, true)

#define SSD1306_LATENCY 10

//! \name OLED controller write and read functions
//@{
/**
 * \brief Writes a command to the display controller
 *
 * This functions pull pin D/C# low before writing to the controller. Different
 * data write function is called based on the selected interface.
 *
 * \param command the command to write
 */
static void ssd1306_write_command(uint8_t command)
{
#if defined(SSD1306_USART_SPI_INTERFACE)
	struct usart_spi_device device = {.id = SSD1306_CS_PIN};
	usart_spi_select_device(SSD1306_USART_SPI, &device);
	ssd1306_sel_cmd();
	usart_spi_transmit(SSD1306_USART_SPI, command);
	usart_spi_deselect_device(SSD1306_USART_SPI, &device);
#elif defined(SSD1306_SPI_INTERFACE)
	struct spi_device device = {.id = SSD1306_CS_PIN};
	spi_select_device(SSD1306_SPI, &device);
	ssd1306_sel_cmd();
	spi_write_single(SSD1306_SPI, command);
	delay_us(SSD1306_LATENCY); // At least 3us
	spi_deselect_device(SSD1306_SPI, &device);
#endif
}

/**
 * \brief Write data to the display controller
 *
 * This functions sets the pin D/C# before writing to the controller. Different
 * data write function is called based on the selected interface.
 *
 * \param data the data to write
 */
static inline void ssd1306_write_data(uint8_t data)
{
#if defined(SSD1306_USART_SPI_INTERFACE)
	struct usart_spi_device device = {.id = SSD1306_CS_PIN};
	usart_spi_select_device(SSD1306_USART_SPI, &device);
	arch_ioport_set_pin_level(SSD1306_DC_PIN, true);
	usart_spi_transmit(SSD1306_USART_SPI, data);
	ssd1306_sel_cmd();
	usart_spi_deselect_device(SSD1306_USART_SPI, &device);
#elif defined(SSD1306_SPI_INTERFACE)
	struct spi_device device = {.id = SSD1306_CS_PIN};
	spi_select_device(SSD1306_SPI, &device);
	ssd1306_sel_data();
	spi_write_single(SSD1306_SPI, data);
	delay_us(SSD1306_LATENCY); // At least 3us
	spi_deselect_device(SSD1306_SPI, &device);
#endif
}

/**
 * \brief Read data from the controller
 *
 * \note The controller does not support read in serial mode.
 *
 * \retval 8 bit data read from the controller
 */
static inline uint8_t ssd1306_read_data(void)
{
	return 0;
}

/**
 * \brief Read status from the controller
 *
 * \note The controller does not support read in serial mode.
 *
 * \retval 8 bit status read from the controller
 */
static inline uint8_t ssd1306_get_status(void)
{
	return 0;
}
//@}

//! \name OLED Controller reset
//@{

/**
 * \brief Perform a hard reset of the OLED controller
 *
 * This functions will reset the OLED controller by setting the reset pin low.
 * \note this functions should not be confused with the \ref ssd1306_soft_reset()
 * function, this command will control the RST pin.
 */
static inline void ssd1306_hard_reset(void)
{
	arch_ioport_set_pin_level(SSD1306_RES_PIN, false);
	// TODO:
    //vTaskDelay(SSD1306_LATENCY / portTICK_PERIOD_MS);  // At least 3us
    vTaskDelay(1 / portTICK_PERIOD_MS);  // At least 3us
	arch_ioport_set_pin_level(SSD1306_RES_PIN, true);
	vTaskDelay(1 / portTICK_PERIOD_MS);  // At least 3us
}
//@}

//! \name Sleep control
//@{
/**
 * \brief Enable the OLED sleep mode
 */
static inline void ssd1306_sleep_enable(void)
{
	ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_OFF);
}

/**
 * \brief Disable the OLED sleep mode
 */
static inline void ssd1306_sleep_disable(void)
{
	ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_ON);
}
//@}

//! \name Address setup for the OLED
//@{
/**
 * \brief Set current page in display RAM
 *
 * This command is usually followed by the configuration of the column address
 * because this scheme will provide access to all locations in the display
 * RAM.
 *
 * \param address the page address
 */
static inline void ssd1306_set_page_address(uint8_t address)
{
	// Make sure that the address is 4 bits (only 8 pages)
	// SST EDIT
	address &= 0x0F;
	ssd1306_write_command(SSD1306_CMD_SET_PAGE_START_ADDRESS(address));
}

/**
 * \brief Set current column in display RAM
 *
 * \param address the column address
 */
static inline void ssd1306_set_column_address(uint8_t address)
{
	// Make sure the address is 7 bits
	address &= 0x7F;
	ssd1306_write_command(SSD1306_CMD_SET_HIGH_COL(address >> 4));
	ssd1306_write_command(SSD1306_CMD_SET_LOW_COL(address & 0x0F));
}

/**
 * \brief Set the display start draw line address
 *
 * This function will set which line should be the start draw line for the OLED.
 */
static inline void ssd1306_set_display_start_line_address(uint8_t address)
{
	// Make sure address is 6 bits
	address &= 0x3F;
	ssd1306_write_command(SSD1306_CMD_SET_START_LINE(address));
}
//@}

//! \name Display hardware control
//@{
/**
 * \brief Turn the OLED display on
 *
 * This function will turn on the OLED.
 */
static inline void ssd1306_display_on(void)
{
	ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_ON);
}

/**
 * \brief Turn the OLED display off
 *
 * This function will turn off the OLED.
 */
static inline void ssd1306_display_off(void)
{
	ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_OFF);
}

/**
 * \brief Set the OLED contrast level
 *
 * \param contrast a number between 0 and 0xFF
 *
 * \retval contrast the contrast value written to the OLED controller
 */
static inline uint8_t ssd1306_set_contrast(uint8_t contrast)
{
	ssd1306_write_command(SSD1306_CMD_SET_CONTRAST_CONTROL_FOR_BANK0);
	ssd1306_write_command(contrast);
	return contrast;
}

/**
 * \brief Invert all pixels on the device
 *
 * This function will invert all pixels on the OLED
 *
 */
static inline void ssd1306_display_invert_enable(void)
{
	ssd1306_write_command(SSD1306_CMD_SET_INVERSE_DISPLAY);
}

/**
 * \brief Disable invert of all pixels on the device
 *
 * This function will disable invert on all pixels on the OLED
 *
 */
static inline void ssd1306_display_invert_disable(void)
{
	ssd1306_write_command(SSD1306_CMD_SET_NORMAL_DISPLAY);
}

static inline void ssd1306_clear(void)
{
	uint8_t page = 0;
	uint8_t col = 0;

	for (page = 0; page < 4; ++page)
	{
		ssd1306_set_page_address(page);
		ssd1306_set_column_address(0);
		for (col = 0; col < 128; ++col)
		{
			ssd1306_write_data(0x00);
		}
	}
}
//@}

//! \name Initialization
//@{
void ssd1306_init(void);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* SSD1306_H_INCLUDED */
