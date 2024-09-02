
#include "gpio_handler.h"
#include "light_driver.h"
#include "smartmeter_reader.h"
#include "zigbee.h"
#include "zigbee_meter.h"

void app_main(void)
{
    zigbee_init();
    light_driver_init(CONFIG_LED_STRIP_PIN);
    config_button(CONFIG_BUTTON_PIN);
    smartmeter_reader_init(CONFIG_UART_RX_PIN, CONFIG_UART_PORT_NUM, CONFIG_UART_BAUDRATE, CONFIG_SMARTMETER_DATA_REQ_PIN);
}
