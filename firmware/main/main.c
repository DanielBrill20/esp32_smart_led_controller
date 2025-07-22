#include "led_strip.h"
#include "esp_err.h"
#include "led.h"
#include "esp_log.h"

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
    // The LED strip object
    led_strip_handle_t led_handle = NULL;
    // Creating the LED strip based on RMT TX channel, checks for errors
    // ESP_ERROR_CHECK checks an esp_err_t value and prints an error message to the console if the value isn't 0 (ESP_OK)
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_handle));

    // Creating instance of my own led struct
    led led = {
        .led_handle = led_handle,
        .index = 0,
        .red = 0,
        .green = 0,
        .blue = 0,
        .state = false,
    };
    int r = 0;
    int g = 0;
    int b = 0;
    int i = 0;

    while(i < 120) {
        led_set_color(&led, r, g, b);
        blink(&led);
        if (i < 20) {
            r++;
        } else if (i < 40) {
            r--;
        } else if (i < 60) {
            g++;
        } else if (i < 80) {
            g--;
        } else if (i < 100) {
            b++;
        } else if (i < 120) {
            b--;
        }
        i++;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}