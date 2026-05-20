idf_component_register(SRCS "bot_crtl.c" "lcd_crtl.c" "main.c" "wifi.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_lcd esp_timer)