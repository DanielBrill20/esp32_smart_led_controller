idf_component_register(SRCS "http_server.c" "wifi_manager.c" "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_wifi
                    REQUIRES esp_http_server
                    REQUIRES nvs_flash
                    REQUIRES esp_netif)