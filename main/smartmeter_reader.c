/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "zigbee_meter.h"
#include "smartmeter_reader.h"
#include "hdlc_data_extractor.h"

#define SMARTMETER_FRAME_SIZE 380 // my smartmeter sends frames of 380 bytes each 5sec

#define BUF_SIZE (1024)

static uint32_t s_uart_rx_pin = 0;
static uint32_t s_uart_port_num = 0;
static uint32_t s_uart_baudrate = 0;
static uint32_t s_smartmeter_data_req_pin = 0;

static void reader_task(void *arg)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = s_uart_baudrate,
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
    io_conf.pin_bit_mask = (1 << s_smartmeter_data_req_pin);
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

    ESP_ERROR_CHECK(uart_driver_install(s_uart_port_num, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(s_uart_port_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(s_uart_port_num, UART_PIN_NO_CHANGE, s_uart_rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    ESP_ERROR_CHECK(uart_set_line_inverse(s_uart_port_num, UART_SIGNAL_RXD_INV));
    ESP_ERROR_CHECK(gpio_set_pull_mode(s_uart_rx_pin, GPIO_PULLUP_ONLY));

    gpio_set_level(s_smartmeter_data_req_pin, 1);

    // Configure a buffer for the incoming data
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);

    // Read loop
    while (1)
    {
        // Read data from the UART
        int len = uart_read_bytes(s_uart_port_num, data, SMARTMETER_FRAME_SIZE, pdMS_TO_TICKS(6000));

        if (len)
        {
            ExtractedData extractedData = {0};
            extractData(data, len, &extractedData);
            zb_update_total_active_power(extractedData.outputW - extractedData.inputW);
            printf("inputW         = %ld\n", extractedData.inputW);
            printf("outputW        = %ld\n", extractedData.outputW);
            printf("totalInputWh   = %ld\n", extractedData.totalInputWh);
            printf("totalOutputWh  = %ld\n", extractedData.totalOutputWh);
        }
    }
}

void smartmeter_reader_init(uint32_t uart_rx_pin, uint32_t uart_port_num, uint32_t uart_baudrate, uint32_t smartmeter_data_req_pin)
{
    s_uart_rx_pin = uart_rx_pin;
    s_uart_port_num = uart_port_num;
    s_uart_baudrate = uart_baudrate;
    s_smartmeter_data_req_pin = smartmeter_data_req_pin;
    xTaskCreate(reader_task, "reader_task", 4096, NULL, 10, NULL);
}
