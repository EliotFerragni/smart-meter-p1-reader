/**
 * taken from https://github.com/Tropaion/ZigBee_SmartMeter_Reader/blob/main/software/zigbee/components/zigbee_electricity_meter/src/zb_electricity_meter.c
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
#define BASIC_MANUFACTURER_NAME             " Eliot Ferragni"  /* < Reserve first char for the size of string */
#define BASIC_MANUFACTURER_NAME_SIZE        0x0E                    /* < Size of BASIC_MANUFACTURER_NAME without first space */
#define BASIC_MODEL_NAME                    " SmartMeter"           /* < Reserve first char for the size of string */
#define BASIC_MODEL_NAME_SIZE               0x0A                    /* < Size of BASIC_MODEL_NAME without first space */
#define BASIC_POWER_SOURCE                  0x04                    /* < DC Power Source */
#define HA_DLMS_ENDPOINT                    1

/* Values for identify cluster */
#define IDENTIFY_IDENTIFY_TIME              ESP_ZB_ZCL_IDENTIFY_IDENTIFY_TIME_DEFAULT_VALUE

/* Values for electrical measurement cluster */
#define ELECTRICAL_MEASUREMENT_RMS_VOLTAGE  0xFFFF                  /* < Default value from specification */
#define ELECTRICAL_MEASUREMENT_RMS_CURRENT  0xFFFF                  /* < Default value from specification */

/* Initial command to request attribute report */
static esp_zb_zcl_report_attr_cmd_t electrical_measurement_cmd_req = {
        .zcl_basic_cmd.src_endpoint = HA_DLMS_ENDPOINT,
        .address_mode = ESP_ZB_APS_ADDR_MODE_DST_ADDR_ENDP_NOT_PRESENT,
        .clusterID = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT,
        .cluster_role = ESP_ZB_ZCL_CLUSTER_SERVER_ROLE
    };

/* ===== FUNCTIONS TO UPDATE AND SEND CLUSTER VALUES ===== */
esp_err_t zb_update_total_active_power(int32_t power)
{
    /* Set attribute request to total active power */
    electrical_measurement_cmd_req.attributeID = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID;

    /* Write new local total active power */
    esp_zb_zcl_status_t state = esp_zb_zcl_set_attribute_val(electrical_measurement_cmd_req.zcl_basic_cmd.src_endpoint,
                                                             electrical_measurement_cmd_req.clusterID,
                                                             electrical_measurement_cmd_req.cluster_role,
                                                             electrical_measurement_cmd_req.attributeID, &power, false);
 
    /* Check for error */
    if(state != ESP_ZB_ZCL_STATUS_SUCCESS)
    {
        ESP_LOGE(TAG, "Setting total active power attribute failed!");
        return ESP_FAIL;
    }

    /* Request sending new total active power */
    state = esp_zb_zcl_report_attr_cmd_req(&electrical_measurement_cmd_req);

    /* Check for error */
    if(state != ESP_ZB_ZCL_STATUS_SUCCESS)
    {
        ESP_LOGE(TAG, "Sending total active power attribute report command failed!");
        return ESP_FAIL;
    }
    
    return ESP_OK;
}


//TODO: Identify Callback
/* ===== FUNCTION TO CREATE ENDPOINTS ===== */
// Create endpoint for electricity meter
void zb_electricity_meter_ep(esp_zb_ep_list_t *esp_zb_ep_list)
{
    /* ===== CREATE BASIC CLUSTER (REQUIRED) (0x0000) ===== */
    esp_zb_attribute_list_t *esp_zb_basic_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_BASIC);

    /* Add attribute zcl_version (0x0000) */
    uint8_t zcl_version = BASIC_ZCL_VERSION;
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_ZCL_VERSION_ID, &zcl_version));

    /* Add attribute application_version (0x0001) */
    uint8_t application_version = BASIC_APPLICATION_VERSION;
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_APPLICATION_VERSION_ID, &application_version));

    /* Add attribute stack_version (0x0002) */
    uint8_t stack_version = BASIC_STACK_VERSION;
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_STACK_VERSION_ID, &stack_version));

    /* Add attribute hw_version (0x0003) */
    uint8_t hw_version = BASIC_HW_VERSION;
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_HW_VERSION_ID, &hw_version));

    /* Add attribute manufacturer_name (0x0004) */
    char manufacturer_name[BASIC_MANUFACTURER_NAME_SIZE + 1] = BASIC_MANUFACTURER_NAME;
    manufacturer_name[0] = BASIC_MANUFACTURER_NAME_SIZE;
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MANUFACTURER_NAME_ID, &manufacturer_name[0]));

    /* Add attribute model_identifier (0x0005) */
    char model_identifier[BASIC_MODEL_NAME_SIZE + 1] = BASIC_MODEL_NAME;
    model_identifier[0] = BASIC_MODEL_NAME_SIZE;
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_MODEL_IDENTIFIER_ID, &model_identifier[0]));

    /* Add attribute power_source (0x0007) */
    uint8_t power_source = BASIC_POWER_SOURCE;
    ESP_ERROR_CHECK(esp_zb_basic_cluster_add_attr(esp_zb_basic_cluster, ESP_ZB_ZCL_ATTR_BASIC_POWER_SOURCE_ID, &power_source));

    /* ===== CREATE IDENTITY CLUSTER (REQUIRED) (0x0003) ===== */
    esp_zb_attribute_list_t *esp_zb_identify_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY);

    /* Add attribute identify_time (0x0000) */
    uint16_t identify_time = IDENTIFY_IDENTIFY_TIME;
    ESP_ERROR_CHECK(esp_zb_identify_cluster_add_attr(esp_zb_identify_cluster, ESP_ZB_ZCL_ATTR_IDENTIFY_IDENTIFY_TIME_ID, &identify_time));

    /* ===== CREATE ELECTRICAL MEASUREMENT CLUSTER (0x0B04)=====*/
    esp_zb_attribute_list_t *esp_zb_electrical_measurement_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT);

    /* == Attribute Set 0x00: Basic Information (S. 299) == */
    //TODO: Add attribute MeasurementType (0x0000) */
    uint32_t measurement_type = (1 << 3);
    ESP_ERROR_CHECK(esp_zb_electrical_meas_cluster_add_attr(esp_zb_electrical_measurement_cluster, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_MEASUREMENT_TYPE_ID, &measurement_type));

    /* == Attribute Set 0x03: AC (Non-phase Specific) Measurements (S. 303) == */
    /* Add attribute TotalActivePower (0x0304) */
    int32_t total_active_power = 0;
    ESP_ERROR_CHECK(esp_zb_electrical_meas_cluster_add_attr(esp_zb_electrical_measurement_cluster, ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID, &total_active_power));

    /* === CREATE METERING CLUSTER (0x0702) === */
    //TODO: Cluster still not implemented in ZigBee SDK: https://github.com/espressif/esp-zigbee-sdk/issues/36

    /* === CREATE CLUSTER CLIENT ROLES === */
    esp_zb_attribute_list_t *esp_zb_identify_client_cluster = esp_zb_zcl_attr_list_create(ESP_ZB_ZCL_CLUSTER_ID_IDENTIFY);

    /* === CREATE CLUSTER LIST === */
    esp_zb_cluster_list_t *esp_zb_cluster_list = esp_zb_zcl_cluster_list_create();

    /* Server clusters */
    ESP_ERROR_CHECK(esp_zb_cluster_list_add_basic_cluster(esp_zb_cluster_list, esp_zb_basic_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));
    ESP_ERROR_CHECK(esp_zb_cluster_list_add_identify_cluster(esp_zb_cluster_list, esp_zb_identify_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));
    ESP_ERROR_CHECK(esp_zb_cluster_list_add_electrical_meas_cluster(esp_zb_cluster_list, esp_zb_electrical_measurement_cluster, ESP_ZB_ZCL_CLUSTER_SERVER_ROLE));

    /* Client clusters */
    ESP_ERROR_CHECK(esp_zb_cluster_list_add_identify_cluster(esp_zb_cluster_list, esp_zb_identify_client_cluster, ESP_ZB_ZCL_CLUSTER_CLIENT_ROLE));

    /* === ADD CREATED ENDPOINT TO LIST === */
    ESP_ERROR_CHECK(esp_zb_ep_list_add_ep(esp_zb_ep_list, esp_zb_cluster_list, HA_DLMS_ENDPOINT, ESP_ZB_AF_HA_PROFILE_ID, ESP_ZB_HA_METER_INTERFACE_DEVICE_ID));

    // /* Config the reporting info  */
    // esp_zb_zcl_reporting_info_t reporting_info = {
    //     .direction = ESP_ZB_ZCL_CMD_DIRECTION_TO_SRV,
    //     .ep = HA_DLMS_ENDPOINT,
    //     .cluster_id = ESP_ZB_ZCL_CLUSTER_ID_ELECTRICAL_MEASUREMENT,
    //     .cluster_role = ESP_ZB_ZCL_CLUSTER_SERVER_ROLE,
    //     .dst.profile_id = ESP_ZB_AF_HA_PROFILE_ID,
    //     .u.send_info.min_interval = 1,
    //     .u.send_info.max_interval = 0,
    //     .u.send_info.def_min_interval = 1,
    //     .u.send_info.def_max_interval = 0,
    //     .u.send_info.delta.u16 = 100,
    //     .attr_id = ESP_ZB_ZCL_ATTR_ELECTRICAL_MEASUREMENT_ACTIVE_POWER_ID,
    //     .manuf_code = ESP_ZB_ZCL_ATTR_NON_MANUFACTURER_SPECIFIC,
    // };

    // esp_zb_zcl_update_reporting_info(&reporting_info);
}