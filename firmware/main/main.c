#include "led_strip.h"
#include "esp_err.h"
#include "led.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "nvs_flash.h"
#include "esp_netif.h"

static const char* WIFI_TAG = "wifi station";
// // LED GPIO
// #define BLINK_GPIO 38

// // LED Strip Config
// static led_strip_config_t strip_config = {
//     .strip_gpio_num = BLINK_GPIO,
//     .max_leds = 1, // 1 onboard LED
//     .led_model = LED_MODEL_WS2812,
//     .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
//     .flags.invert_out = false,
// };

// // RMT Backend Configuration
// static led_strip_rmt_config_t rmt_config = {
//     .clk_src = RMT_CLK_SRC_DEFAULT,
//     .resolution_hz = 10 * 1000 * 1000, // RMT counter clock frequency: 10MHz
//     .mem_block_symbols = 64,           // the memory size of each RMT channel, in words (4 bytes)
//     .flags.with_dma = false,
// };

// static void led_set_color(led* led, int red, int green, int blue)
// {
//     led->red = red;
//     led->green = green;
//     led->blue = blue;
// }

// static void blink(led* led)
// {
//     if (led->state) {
//         // Turns off the LED strip
//         ESP_ERROR_CHECK(led_strip_clear(led->led_handle));
//         ESP_LOGI("", "LED Off");
//     } else {
//         // Set the color of the LED at the 0th index
//         ESP_ERROR_CHECK(led_strip_set_pixel(led->led_handle, led->index, led->red, led->green, led->blue));
//         // Push the LED color out to the device
//         ESP_ERROR_CHECK(led_strip_refresh(led->led_handle));
//         ESP_LOGI("", "Outputting color (%d, %d, %d)", led->red, led->green, led->blue);
//     }
//     led->state = !led->state;
// }

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
        ESP_LOGI(WIFI_TAG, "Connection to AP started successfully");
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(WIFI_TAG, "Failed to connect to the AP");
        // Add disconnection logic, maybe try reconnecting up to a certain number of attempts
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI(WIFI_TAG, "Internal connection successful, starting DHCP client, no action needed.");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(WIFI_TAG, "Successfully retreived IP address:" IPSTR, IP2STR(&event->ip_info.ip));
    }
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &event_handler,
            NULL,
            &instance_any_id
        )
    );
    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &event_handler,
            NULL,
            &instance_got_ip
        )
    );

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    ESP_ERROR_CHECK(esp_wifi_start());
    // // The LED strip object
    // led_strip_handle_t led_handle = NULL;
    // // Creating the LED strip based on RMT TX channel, checks for errors
    // // ESP_ERROR_CHECK checks an esp_err_t value and prints an error message to the console if the value isn't 0 (ESP_OK)
    // ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_handle));

    // // Creating instance of my own led struct
    // led led = {
    //     .led_handle = led_handle,
    //     .index = 0,
    //     .red = 0,
    //     .green = 0,
    //     .blue = 0,
    //     .state = false,
    // };
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