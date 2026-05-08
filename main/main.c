
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <time.h>
#include "esp_system.h"
#include "esp_mac.h"

#include "esp_lcd_ili9341.h"

#include "esp_lcd_panel_commands.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"

#include <sys/lock.h>
#include <sys/param.h>
#include "esp_timer.h"
#include "esp_log.h"

#include "driver/gpio.h"
#include "esp_vfs_dev.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lcd_crtl.h"
#include "bot_crtl.h"

#define PROBABILITY_CONTROLL 1000
static const char *TAG = "BOT_HAPPY";
TaskHandle_t display_controller_handle = NULL;
TaskHandle_t state_controller_hande = NULL;
static void display_controller(void* param);
static void state_controller(void* param);

void app_main(void)
{
    srand(time(NULL));
    
    lcd_crtl_display_init();
    vTaskDelay(pdMS_TO_TICKS(500));
    bot_init();
    xTaskCreatePinnedToCore(state_controller,"State_Controller",4098,NULL,2,&state_controller_hande,0);

}

static void state_controller(void* param)
{
    
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(100);
    BaseType_t xWasDelayed;

    for(;;){
       
            uint16_t animation_probability = (rand() % PROBABILITY_CONTROLL);
            if(animation_probability >= 0 && animation_probability < 250){
                bot_idle_animation();
                ESP_LOGI(TAG, "Bot on Idle");
            }
            else if(animation_probability >= 250 && animation_probability < 375){
                bot_blink_animation();
                ESP_LOGI(TAG, "Bot blinking\n");

            }
            else if(animation_probability >= 375 && animation_probability < 500){
                bot_side_watch_animation();
                ESP_LOGI(TAG, "Bot spying(side watch)\n");

            }
            else if(animation_probability >= 500 && animation_probability < 625){
                bot_talking_animation();
                ESP_LOGI(TAG, "Bot talking\n");

            }
            else if(animation_probability >= 625 && animation_probability < 750){
                bot_moving_anthena_animation();
                ESP_LOGI(TAG, "Bot moving antena side to side");

            }
            else if(animation_probability >= 750 && animation_probability < 875){
                bot_happy_animation();
                ESP_LOGI(TAG, "bot is HAPPY");
            }
            else{
                bot_dead_eyes_animation();
                ESP_LOGI(TAG, "Bot is committed Poli[suicide]");
            }
        }
    }


/******************************************************************************/
/* End of file ****************************************************************/

