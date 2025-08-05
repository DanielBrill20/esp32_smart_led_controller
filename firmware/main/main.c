#include "esp_err.h"
#include "esp_log.h"
#include "led_strip.h"
#include "led.h"
#include "wifi_manager.h"
#include "http_server.h"

static const char* LED_TAG = "led strip";

// LED GPIO
#define BLINK_GPIO 38

// LED Strip Config
static led_strip_config_t strip_config = {
    .strip_gpio_num = BLINK_GPIO,
    .max_leds = 1, // 1 onboard LED
    .led_model = LED_MODEL_WS2812,
    .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
    .flags.invert_out = false,
};

// RMT Backend Configuration
static led_strip_rmt_config_t rmt_config = {
    .clk_src = RMT_CLK_SRC_DEFAULT,
    .resolution_hz = 10 * 1000 * 1000, // RMT counter clock frequency: 10MHz
    .mem_block_symbols = 64,           // the memory size of each RMT channel, in words (4 bytes)
    .flags.with_dma = false,
};

static void led_set_color(led* led, int red, int green, int blue)
{
    led->red = red;
    led->green = green;
    led->blue = blue;
}

static void blink(led* led)
{
    if (led->state) {
        // Turns off the LED strip
        ESP_ERROR_CHECK(led_strip_clear(led->led_handle));
        ESP_LOGI("", "LED Off");
    } else {
        // Set the color of the LED at the 0th index
        ESP_ERROR_CHECK(led_strip_set_pixel(led->led_handle, led->index, led->red, led->green, led->blue));
        // Push the LED color out to the device
        ESP_ERROR_CHECK(led_strip_refresh(led->led_handle));
        ESP_LOGI("", "Outputting color (%d, %d, %d)", led->red, led->green, led->blue);
    }
    led->state = !led->state;
}

void app_main(void)
{
    wifi_manager_init();
    http_server_init();
    // int r = 0;
    // int g = 0;
    // int b = 0;
    // int i = 0;

    // while(i < 120) {
    //     led_set_color(&led, r, g, b);
    //     blink(&led);
    //     if (i < 20) {
    //         r++;
    //     } else if (i < 40) {
    //         r--;
    //     } else if (i < 60) {
    //         g++;
    //     } else if (i < 80) {
    //         g--;
    //     } else if (i < 100) {
    //         b++;
    //     } else if (i < 120) {
    //         b--;
    //     }
    //     i++;
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }
}
