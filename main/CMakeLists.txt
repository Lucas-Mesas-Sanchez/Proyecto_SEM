idf_component_register(SRCS "lcd_crtl.c" "pola.c" "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_lcd esp_timer)