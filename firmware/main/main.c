#include "esp_err.h"
#include "esp_log.h"
#include "led_strip.h"
#include "wifi_manager.h"
#include "http_server.h"

void app_main(void)
{
    wifi_manager_init();
    http_server_init();
    led_manager_init();
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
}
