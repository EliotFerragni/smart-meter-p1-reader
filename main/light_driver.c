#include "esp_log.h"
#include "led_strip.h"
#include "light_driver.h"

static const char *TAG = "light_driver";

#define NUM_LED_ESP32_H2_DEVKIT 1

static led_strip_handle_t s_led_strip;

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} rgb_color;

static rgb_color colors[] = {
    {
        // RED
        .red = 255,
        .green = 0,
        .blue = 0,
    },
    {
        // GREEN
        .red = 0,
        .green = 255,
        .blue = 0,
    },
    {
        // BLUE
        .red = 0,
        .green = 0,
        .blue = 255,
    },
    {
        // YELLOW
        .red = 255,
        .green = 255,
        .blue = 0,
    },
    {
        // LIGHT_BLUE
        .red = 0,
        .green = 255,
        .blue = 255,
    },
    {
        // MAGENTA
        .red = 255,
        .green = 0,
        .blue = 255,
    },
    {
        // WHITE
        .red = 255,
        .green = 255,
        .blue = 255,
    },
};

void light_driver_set_led(color color, uint8_t intensity)
{
    if (color >= NB_COLORS)
    {
        ESP_LOGE(TAG, "Invalid color %d", color);
        return;
    }

    intensity = intensity > 100 ? 100 : intensity;
    float new_intensity = (float)intensity / 100;

    const rgb_color *new_color = &colors[color];

    ESP_ERROR_CHECK(led_strip_set_pixel(s_led_strip, 0, new_color->red * new_intensity, new_color->green * new_intensity, new_color->blue * new_intensity));
    ESP_ERROR_CHECK(led_strip_refresh(s_led_strip));
}

void light_driver_init(uint32_t gpio)
{
    led_strip_config_t led_strip_conf = {
        .max_leds = NUM_LED_ESP32_H2_DEVKIT,
        .strip_gpio_num = gpio,
    };
    led_strip_rmt_config_t rmt_conf = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&led_strip_conf, &rmt_conf, &s_led_strip));
    light_driver_set_led(WHITE, 0);
}
