/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "zigbee_meter.h"
#include "smartmeter_reader.h"

#include "hdlc_parser.h"

#define UART_TXD (UART_PIN_NO_CHANGE)
#define UART_RXD (GPIO_NUM_22)
#define UART_RTS (UART_PIN_NO_CHANGE)
#define UART_CTS (UART_PIN_NO_CHANGE)

#define DATA_REQ_GPIO (GPIO_NUM_8)

#define UART_PORT_NUM (1)

#define SMARTMETER_FRAME_SIZE 380 // my smartmeter sends frames of 380 bytes each 5sec

#define BUF_SIZE (1024)

static void reader_task(void *arg)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    int intr_alloc_flags = 0;

    // zero-initialize the config structure.
    gpio_config_t io_conf = {};
    // bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1 << DATA_REQ_GPIO);
    // disable pull-down mode
    io_conf.pull_down_en = 0;
    // no interrupt
    io_conf.intr_type = GPIO_INTR_DISABLE;
    // set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    // disable pull-up mode
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_PORT_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_PORT_NUM, UART_TXD, UART_RXD, UART_RTS, UART_CTS));
    ESP_ERROR_CHECK(uart_set_line_inverse(UART_PORT_NUM, UART_SIGNAL_RXD_INV));
    ESP_ERROR_CHECK(gpio_set_pull_mode(UART_RXD, GPIO_PULLUP_ONLY));

    gpio_set_level(DATA_REQ_GPIO, 1);

    // Configure a buffer for the incoming data
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);

    // Read loop
    while (1)
    {
        // Read data from the UART
        int len = uart_read_bytes(UART_PORT_NUM, data, SMARTMETER_FRAME_SIZE, pdMS_TO_TICKS(6000));

        if (len)
        {
            DataParsed parsedData = {0};
            parseHdlc(data, len, &parsedData);
            zb_update_total_active_power(parsedData.outputW - parsedData.inputW);
        }
    }
}

void smartmeter_reader_init(void)
{
    xTaskCreate(reader_task, "reader_task", 4096, NULL, 10, NULL);
}
