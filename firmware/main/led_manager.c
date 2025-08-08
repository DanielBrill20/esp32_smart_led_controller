#include "led_manager.h"

#define MICRO_PER_MILLI 1000
#define DOT_MS 100
#define DASH_MS (DOT_MS * 3)
#define DOT_DASH_SEP_MS DOT_MS
#define CHAR_SEP_MS DASH_MS
#define WORD_SEP_MS (DOT_MS * 7)

static const char* LED_TAG = "led strip";

enum {
    RED,
    GREEN,
    BLUE
};

// The LED strip object
static led_strip_handle_t led_handle;

static esp_timer_handle_t blinky_timer;
static esp_timer_handle_t morse_code_timer;

typedef struct {
    led_mode_t mode;
    bool state;
    uint32_t blink_duration;
    char* morse_code;
    uint8_t rgb[3];
} led_config_t;

struct led_t {
    led_strip_handle_t led_handle;
    uint8_t index;
    led_config_t config;
};

// LED Strip Config
static led_strip_config_t strip_config = {
    .strip_gpio_num = CONFIG_LED_GPIO,
    .max_leds = CONFIG_MAX_LEDS,
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
    .state = OFF,
    .rgb = {25, 25, 25}
};

static void activate_light(led_t* led)
{
    if (led->config.state) {
        ESP_ERROR_CHECK(
            led_strip_set_pixel(
                led->led_handle,
                led->index,
                led->config.rgb[RED],
                led->config.rgb[GREEN],
                led->config.rgb[BLUE]
            )
        );
        // Push the LED color out to the device
        ESP_ERROR_CHECK(led_strip_refresh(led->led_handle));
        ESP_LOGI(LED_TAG, "LED On");
    } else {
        // Turns off the LED strip
        ESP_ERROR_CHECK(led_strip_clear(led->led_handle));
        ESP_LOGI(LED_TAG, "LED Off");
    }
}

static void activate_blinky(led_t* led)
{
    ESP_ERROR_CHECK(
        esp_timer_start_periodic(
            blinky_timer,
            led->config.blink_duration * MICRO_PER_MILLI
        )
    );
    ESP_LOGI(LED_TAG, "Blinking LED with duration %" PRIu32 " ms", led->config.blink_duration);
}

static void activate_morse()
{
    ESP_ERROR_CHECK(esp_timer_start_once(morse_code_timer, 0));
}

static void blinky_timer_callback(void* arg)
{
    led_t* led = (led_t*)arg;
    set_led_state(led, !led->config.state);
    activate_light(led);
}

static void morse_blink(morse_iterator_t* iterator, bool state, int milliseconds)
{
    set_led_state(iterator->led, state);
    activate_light(iterator->led);
    ESP_ERROR_CHECK(
        esp_timer_start_once(
            morse_code_timer,
            milliseconds * MICRO_PER_MILLI
        )
    );
}

static bool is_gap(char current_char, char next_char)
{
    return (current_char == '.' || current_char == '-') &&
            (next_char == '.' || next_char == '-');
}

static void morse_code_timer_callback(void* arg)
{
    morse_iterator_t* iterator = (morse_iterator_t*)arg;

    // Setting gap and str_len at start of morse code string
    if (iterator->index == 0) {
        iterator->gap = false;
        iterator->str_len = strlen(iterator->led->config.morse_code);
        ESP_LOGI(LED_TAG, "Starting to blink morse code string %s", iterator->led->config.morse_code);
    }

    // Stop timer (by not starting again) and reset index and light once end of morse code string reached
    if (iterator->index > iterator->str_len - 1) {
        set_led_state(iterator->led, OFF);
        activate_light(iterator->led);
        iterator->index = 0;
        ESP_LOGI(LED_TAG, "Finished morse code");
        return;
    }

    // Set off between dots and dashes
    if (iterator->gap) {
        morse_blink(iterator, OFF, DOT_DASH_SEP_MS);
        // Reset gap once gap execution finished
        iterator->gap = false;
        ESP_LOGI(LED_TAG, "Pausing %d ms for gap", DOT_DASH_SEP_MS);
        return;
    }

    char current_char = iterator->led->config.morse_code[iterator->index];
    switch (current_char) {
        case '.':
            morse_blink(iterator, ON, DOT_MS);
            ESP_LOGI(LED_TAG, "Blinking %d ms for dot", DOT_MS);
            break;
        case '-':
            morse_blink(iterator, ON, DASH_MS);
            ESP_LOGI(LED_TAG, "Blinking %d ms for dash", DASH_MS);
            break;
        case ' ':
            morse_blink(iterator, OFF, CHAR_SEP_MS);
            ESP_LOGI(LED_TAG, "Pausing %d ms before next English character", CHAR_SEP_MS);
            break;
        case '/':
            morse_blink(iterator, OFF, WORD_SEP_MS);
            ESP_LOGI(LED_TAG, "Pausing %d ms before next word", WORD_SEP_MS);
            break;
        default:
            ESP_LOGE(LED_TAG, "Unknown character in morse code string");
    }
    // Increase index if a character was just read
    iterator->index++;
    // Set gap to true if we just read a . or - and are about to read another
    if (iterator->index < iterator->str_len) {
        char next_char = iterator->led->config.morse_code[iterator->index];
        if (is_gap(current_char, next_char)) {
            iterator->gap = true;
        }
    }
}

led_t* create_led(uint8_t index)
{
    led_t* led = malloc(sizeof(led_t));
    if (!led) return NULL;
    led->led_handle = led_handle;
    led->index = index;
    led->config = config;
    return led;
}

esp_err_t destroy_led(led_t* led)
{
    if (!led) return ESP_FAIL;
    if (led->config.morse_code) {
        free(led->config.morse_code);
    }
    free(led);
    return ESP_OK;
}

void set_led_mode(led_t* led, led_mode_t mode)
{
    // Returns ESP_ERR_INVALID_STATE if timer is not running
    // It won't always be running, but this doesn't seem to cause issues/stop execution
    esp_timer_stop(blinky_timer);
    esp_timer_stop(morse_code_timer);

    led->config.mode = mode;
    switch (mode) {
        case LED_MODE_LIGHT:
            activate_light(led);
            break;
        case LED_MODE_BLINKY:
            activate_blinky(led);
            break;
        case LED_MODE_MORSE:
            activate_morse();
            break;
        default:
            ESP_LOGE(LED_TAG, "Unknown LED mode");
    }
}

void set_led_state(led_t* led, bool state)
{
    led->config.state = state;
}

void set_led_blink_duration(led_t* led, uint32_t blink_duration)
{
    led->config.blink_duration = blink_duration;
}

void set_led_morse_code(led_t* led, char* morse_code)
{
    // Free old morse_code string if it exists
    if (led->config.morse_code) {
        free(led->config.morse_code);
    }
    led->config.morse_code = morse_code;
}

void set_led_rgb(led_t* led, uint8_t red, uint8_t green, uint8_t blue)
{
    uint8_t rgb_array[3] = {red, green, blue};
    memcpy(led->config.rgb, rgb_array, 3);

    if (led->config.state == ON) {
        activate_light(led);
    }
    ESP_LOGI(LED_TAG, "Set LED color to R: %u, G: %u, B: %u", red, green, blue);
}

void led_timers_init(led_t* led, morse_iterator_t* morse_iterator)
{
    const esp_timer_create_args_t blinky_timer_args = {
        .callback = blinky_timer_callback,
        .arg = led,
        .name = "periodic"
    };
    const esp_timer_create_args_t morse_code_timer_args = {
        .callback = morse_code_timer_callback,
        .arg = morse_iterator,
        .name = "oneshot"
    };
    ESP_ERROR_CHECK(esp_timer_create(&blinky_timer_args, &blinky_timer));
    ESP_ERROR_CHECK(esp_timer_create(&morse_code_timer_args, &morse_code_timer));
}

void led_manager_init()
{
    // Creating the LED strip based on RMT TX channel, checks for errors
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_handle));
}
