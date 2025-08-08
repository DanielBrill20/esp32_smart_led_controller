idf_component_register(SRCS "led_manager.c" "http_server.c" "wifi_manager.c" "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi esp_http_server nvs_flash esp_netif json esp_timer)
