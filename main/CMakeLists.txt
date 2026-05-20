idf_component_register(SRCS "http_server.c" "Wifi.c" "bot_crtl.c" "lcd_crtl.c" "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_lcd esp_timer esp_wifi esp_event nvs_flash esp_http_server)