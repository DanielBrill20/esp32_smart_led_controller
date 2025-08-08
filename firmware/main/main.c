#include "esp_err.h"
#include "esp_log.h"
#include "led_strip.h"
#include "wifi_manager.h"
#include "http_server.h"
#include "led_manager.h"

void app_main(void)
{
    /*
    * Note: WiFi connection is asynchronous (includes background tasks),
    * so it must use xEventGroupWaitBits to ensure connection finishes
    * prior to starting the http server.
    * However, led_manager, which initializes the led_strip_handle_t
    * needed in http server, is synchronous, so it automatically blocks
    * and prevents http_server_init from running until itself is finished.
    * Therefore, no wait bits are needed, but led_manager_init must
    * precede http_server_init
    */
    wifi_manager_init();
    led_manager_init();
    http_server_init();
}
