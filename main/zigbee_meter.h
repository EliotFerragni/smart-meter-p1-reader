#pragma once

/* Zigbee Library */
#include "esp_zigbee_core.h"

/**
 * @brief Set and send the new power given in parameter
 * 
 * @param power 
 * @return esp_err_t 
 */
void zb_electricity_meter_ep(esp_zb_ep_list_t *esp_zb_ep_list);

/**
 * @brief Create endpoint for electrical measurement
 * 
 * @param esp_zb_ep_list List used to add the endpoint
 */
esp_err_t zb_update_total_active_power(int32_t power);