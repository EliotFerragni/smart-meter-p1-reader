#pragma once

/* Zigbee Library */
#include "esp_zigbee_core.h"


/**
 * @brief Create endpoint for electrical measurement
 * 
 * @param esp_zb_ep_list List used to add the endpoint
 */
void zb_electrical_measurement_ep(esp_zb_ep_list_t *esp_zb_ep_list);

/**
 * @brief Create endpoint for metering
 * 
 * @param esp_zb_ep_list List used to add the endpoint
 */
void zb_metering_ep(esp_zb_ep_list_t *esp_zb_ep_list);

/**
 * @brief Set and send the new power given in parameter
 * 
 * @param power in W
 * @return esp_err_t 
 */
esp_err_t zb_update_total_active_power(int32_t power);

/**
 * @brief Set and send the new delivered power given in parameter
 * 
 * @param delivered_power in Wh 
 * @return esp_err_t 
 */
esp_err_t zb_update_delivered_power(uint64_t delivered_power);

/**
 * @brief Set and send the new received power given in parameter
 * 
 * @param received_power in Wh 
 * @return esp_err_t 
 */
esp_err_t zb_update_received_power(uint64_t received_power);