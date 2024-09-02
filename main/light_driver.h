
#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    RED,
    GREEN,
    BLUE,
    YELLOW,
    LIGHT_BLUE,
	MAGENTA,
	WHITE,
    NB_COLORS
} color;

/**
 * @brief Set led color
 * 
 * @param color New color. See color type
 * @param intensity New intensity. Percentage between 0-100 
 */
void light_driver_set_led(color color, uint8_t intensity);

/**
 * @brief color light driver init, be invoked where you want to use color light
 *
 * @param power power on/off
 */
void light_driver_init(uint32_t gpio);
