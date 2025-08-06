#include "led_manager.h"

static const char* LED_TAG = "led strip";
static led_t led;

// LED Strip Config
static led_strip_config_t strip_config = {
    .strip_gpio_num = CONFIG_LED_GPIO,
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

// My Led Struct Configuration
static led_config_t config = {
    .mode = LED_MODE_LIGHT,
    .state = false,
    .rgb = {25, 25, 25}
};

static void set_led_state(led_t* led)
{
    if (led->config.state) {
        ESP_LOGI(LED_TAG, "State is true, turning on");
        ESP_ERROR_CHECK(
            led_strip_set_pixel(
                led->led_handle,
                led->index,
                led->config.rgb[RED],
                led->config.rgb[GREEN],
                led->config.rgb[BLUE]
            )
        );
         ESP_LOGI(LED_TAG, "Set, now pushing");
        // Push the LED color out to the device
        ESP_ERROR_CHECK(led_strip_refresh(led->led_handle));
        ESP_LOGI(LED_TAG, "Turning on LED");
    } else {
        // Turns off the LED strip
        ESP_ERROR_CHECK(led_strip_clear(led->led_handle));
        ESP_LOGI(LED_TAG, "Turning off LED");
    }
}

static void blink_led(led_t* led)
{
    led->config.state = !led->config.state;
    set_led_state(led);
}

static void set_mode(led_t* led, led_mode_t mode)
{
    ESP_LOGI(LED_TAG, "Entered set mode function");
    led->config.mode = mode;

    switch (mode) {
        case LED_MODE_LIGHT:
            ESP_LOGI(LED_TAG, "Received mode, about to set led");
            set_led_state(led);
            break;
        case LED_MODE_BLINKY:
            //pass
            break;
        case LED_MODE_MORSE:
            //pass
            break;
        default:
            ESP_LOGE(LED_TAG, "Unknown LED mode");
    }
}

static void set_rgb(led_t* led, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t rgb_array[3] = {red, green, blue};
    memcpy(led->config.rgb, rgb_array, 3);
}

led_t* led_manager_get_led()
{
    ESP_LOGI(LED_TAG, "Returning LED obj");
    return &led;
}

void led_manager_init()
{
    // The LED strip object
    led_strip_handle_t led_handle = NULL;
    // Creating the LED strip based on RMT TX channel, checks for errors
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_handle));
    led = (led_t){
        .led_handle = led_handle,
        .index = 0,
        .config = config,
        .set_mode = set_mode,
        .set_rgb = set_rgb
    };
}