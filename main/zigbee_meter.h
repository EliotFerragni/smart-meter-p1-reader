#pragma once

#ifdef __cplusplus
extern "C" {
#endif
/* Zigbee Library */
#include "esp_zigbee_core.h"

void zb_electricity_meter_ep(esp_zb_ep_list_t *esp_zb_ep_list);

esp_err_t zb_update_total_active_power(int32_t power);
#ifdef __cplusplus
} // extern "C"
#endif