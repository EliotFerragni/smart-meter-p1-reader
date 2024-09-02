#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "zigbee_meter.h"

#include "gpio_handler.h"

static QueueHandle_t s_gpio_evt_queue = NULL;
static uint32_t s_gpio = 0;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    xQueueSendFromISR(s_gpio_evt_queue, &s_gpio, NULL);
}

// used to test the zigbee connection by changing the value when pressing the button
static void gpio_task(void* arg)
{
    uint32_t io_num;
    uint32_t power = -20;
    for (;;) {
        if (xQueueReceive(s_gpio_evt_queue, &io_num, portMAX_DELAY)) {
            zb_update_total_active_power(power++);
        }
    }
}

void config_button(uint32_t gpio)
{
    s_gpio = gpio;

    //zero-initialize the config structure.
    gpio_config_t io_conf = {};
    //bit mask of the pins that you want to set,e.g.GPIO18/19
    io_conf.pin_bit_mask = (1 << s_gpio);
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //create a queue to handle gpio event from isr
    s_gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //start gpio task
    xTaskCreate(gpio_task, "gpio_task", 4096, NULL, 10, NULL);

    //install gpio isr service
    gpio_install_isr_service(0);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(s_gpio, gpio_isr_handler, NULL);

    printf("Minimum free heap size: %"PRIu32" bytes\n", esp_get_minimum_free_heap_size());
}
