#include "esp_log.h"
#include "led_strip.h"
#include "light_driver.h"

static led_strip_handle_t s_led_strip;
static uint8_t s_red = 255, s_green = 255, s_blue = 255;
static bool s_enable = false;

void light_driver_enable(bool enable)
{
    s_enable = enable;
    ESP_ERROR_CHECK(led_strip_set_pixel(s_led_strip, 0, s_red * enable, s_green * enable, s_blue * enable));
    ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
}

bool light_driver_get_enable(void)
{
    return s_enable;
}

void light_driver_init(bool enable, uint32_t gpio, uint32_t num_leds)
{
    led_strip_config_t led_strip_conf = {
        .max_leds = num_leds,
        .strip_gpio_num = gpio,
    };
    led_strip_rmt_config_t rmt_conf = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&led_strip_conf, &rmt_conf, &s_led_strip));
    light_driver_enable(enable);
}
