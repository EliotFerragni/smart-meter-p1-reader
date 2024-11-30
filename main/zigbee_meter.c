/**
 * Inspired by https://github.com/Tropaion/ZigBee_SmartMeter_Reader/blob/main/software/zigbee/components/zigbee_electricity_meter/src/zb_electricity_meter.c
 */

#include <stdio.h>

/* Setup logging */
#include "esp_log.h"
static const char* TAG = "zb_meter";

#include "zigbee_meter.h"

/* Values for basic cluster */
#define BASIC_ZCL_VERSION                   ESP_ZB_ZCL_BASIC_ZCL_VERSION_DEFAULT_VALUE
#define BASIC_APPLICATION_VERSION           ESP_ZB_ZCL_BASIC_APPLICATION_VERSION_DEFAULT_VALUE
#define BASIC_STACK_VERSION                 ESP_ZB_ZCL_BASIC_STACK_VERSION_DEFAULT_VALUE
#define BASIC_HW_VERSION                    ESP_ZB_ZCL_BASIC_HW_VERSION_DEFAULT_VALUE
#define BASIC_MANUFACTURER_NAME             " Eliot Ferragni"                       /* < Reserve first char for the size of string */
#define BASIC_MANUFACTURER_NAME_SIZE        (sizeof(BASIC_MANUFACTURER_NAME) - 2)   /* < Size of BASIC_MANUFACTURER_NAME without first space */
#define BASIC_MODEL_NAME                    " SmartMeter"                           /* < Reserve first char for the size of string */
#define BASIC_MODEL_NAME_SIZE               (sizeof(BASIC_MODEL_NAME) - 2)          /* < Size of BASIC_MODEL_NAME without first space */
#define BASIC_POWER_SOURCE                  0x04                                    /* < DC Power Source */
#define HA_DLMS_ENDPOINT                    1

/* Values for identify cluster */
#define IDENTIFY_IDENTIFY_TIME              ESP_ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE


/* Initial command to request attribute report */
static esp_zb_zcl_report_attr_cmd_t electrical_measurement_cmd_req = {
        .zcl_basic_cmd.src_endpoint = HA_DLMS_ENDPOINT,
        .address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT,
        .clusterID = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT,
        .direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_CLI
    };

esp_err_t zb_update_total_active_power(int32_t power)
{
    /* Set attribute request to active power */
    electrical_measurement_cmd_req.attributeID = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID;

    /* Write new local active power */
    esp_zb_zcl_status_t state = esp_zb_zcl_set_attribute_val(HA_DLMS_ENDPOINT,
                                                             ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT,
                                                             ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
                                                             ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID, &power, false);
 
    /* Check for error */
    if(state != ESP_ZB_ZCL_STATUS_SUCCESS)
    {
        ESP_LOGE(TAG, "Setting active power attribute failed!");
        return ESP_FAIL;
    }

    /* Request sending new total active power */
    state = esp_zb_zcl_report_attr_cmd_req(&electrical_measurement_cmd_req);

    /* Check for error */
    if(state != ESP_ZB_ZCL_STATUS_SUCCESS)
    {
        ESP_LOGE(TAG, "Sending active power attribute report command failed!");
        return ESP_FAIL;
    }
    
    return ESP_OK;
}

void zb_electricity_meter_ep(esp_zb_ep_list_t *esp_zb_ep_list)
{   
    // cluster list
    esp_zb_cluster_list_t *esp_zb_cluster_list = esp_zb_zcl_cluster_list_create();

    // basic cluster
    esp_zb_attribute_list_t *esp_zb_basic_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_BASIC);
    uint8_t zcl_version = BASIC_ZCL_VERSION;
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, &zcl_version));
    uint8_t application_version = BASIC_APPLICATION_VERSION;
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID, &application_version));
    uint8_t stack_version = BASIC_STACK_VERSION;
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID, &stack_version));
    uint8_t hw_version = BASIC_HW_VERSION;
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_HW_VERSION_ID, &hw_version));
    char manufacturer_name[BASIC_MANUFACTURER_NAME_SIZE + 1] = BASIC_MANUFACTURER_NAME;
    manufacturer_name[0] = BASIC_MANUFACTURER_NAME_SIZE;
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, &manufacturer_name[0]));
    char model_identifier[BASIC_MODEL_NAME_SIZE + 1] = BASIC_MODEL_NAME;
    model_identifier[0] = BASIC_MODEL_NAME_SIZE;
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, &model_identifier[0]));
    uint8_t power_source = BASIC_POWER_SOURCE;
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, &power_source));
    ESP_ERROR_CHECK(esp_zb_cluster_list_add_basic_cluster(esp_zb_cluster_list, esp_zb_basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));

    // identify cluster
    esp_zb_attribute_list_t *esp_zb_identify_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY);
    uint16_t identify_time = IDENTIFY_IDENTIFY_TIME;
    ESP_ERROR_CHECK(esp_zb_identify_cluster_add_attr(esp_zb_identify_cluster, ESP_ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID, &identify_time));
    ESP_ERROR_CHECK(esp_zb_cluster_list_add_identify_cluster(esp_zb_cluster_list, esp_zb_identify_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));

    // electrical measurement cluster
    esp_zb_attribute_list_t *esp_zb_electrical_measurement_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT);
    // Table 4-28. MeasurementType Attribute from https://zigbeealliance.org/wp-content/uploads/2019/12/07-5123-06-zigbee-cluster-library-specification.pdf
    uint32_t measurement_type = (1 << 3);
    ESP_ERROR_CHECK(esp_zb_electrical_meas_cluster_add_attr(esp_zb_electrical_measurement_cluster, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID, &measurement_type));
    int32_t active_power = 0;
    ESP_ERROR_CHECK(esp_zb_electrical_meas_cluster_add_attr(esp_zb_electrical_measurement_cluster, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID, &active_power));
    ESP_ERROR_CHECK(esp_zb_cluster_list_add_electrical_meas_cluster(esp_zb_cluster_list, esp_zb_electrical_measurement_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));

    // identify client cluster
    esp_zb_attribute_list_t *esp_zb_identify_client_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY);
    ESP_ERROR_CHECK(esp_zb_cluster_list_add_identify_cluster(esp_zb_cluster_list, esp_zb_identify_client_cluster, ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE));

    esp_zb_endpoint_config_t ep_config;
    ep_config.app_device_id = ESP_ZB_HA_METER_INTERFACE_DEVICE_ID;
    ep_config.app_device_version = 1;
    ep_config.app_profile_id = ESP_ZB_AF_HA_PROFILE_ID;
    ep_config.endpoint = HA_DLMS_ENDPOINT;
    // Add everything to endpoint
    ESP_ERROR_CHECK(esp_zb_ep_list_add_ep(esp_zb_ep_list, esp_zb_cluster_list, ep_config));
}