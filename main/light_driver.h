
#pragma once

#include <stdbool.h>
/**
 * @brief Set light power (on/off).
 *
 * @param  power  The light power to be set
 */
void light_driver_enable(bool enable);

bool light_driver_get_enable(void);

/**
 * @brief color light driver init, be invoked where you want to use color light
 *
 * @param power power on/off
 */
void light_driver_init(bool enable, uint32_t gpio, uint32_t num_leds);
