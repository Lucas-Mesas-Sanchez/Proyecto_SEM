
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "esp_system.h"
#include "esp_mac.h"

#include "esp_timer.h"
#include "esp_log.h"

#include "esp_vfs_dev.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lcd_crtl.h"
#include "bot_crtl.h"

#define PROBABILITY_CONTROLL 1000
static const char *TAG = "BOT_HAPPY";
TaskHandle_t state_controller_hande = NULL;
static void state_controller(void* param);

void app_main(void)
{
    srand(time(NULL));
    
    lcd_crtl_display_init();
    vTaskDelay(pdMS_TO_TICKS(500));
    bot_init();
    xTaskCreatePinnedToCore(state_controller,"State_Controller",8192,NULL,2,&state_controller_hande,0);

}

static void state_controller(void* param)
{
    
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(100);
    BaseType_t xWasDelayed;

    for(;;){
       
            uint16_t animation_probability = (rand() % PROBABILITY_CONTROLL);
            if(animation_probability < 250){
                ESP_LOGI(TAG, "Bot on Idle");
                bot_idle_animation();
                
            }
            else if(animation_probability >= 250 && animation_probability < 375){
                ESP_LOGI(TAG, "Bot blinking\n");
                bot_blink_animation();
               
            }
            else if(animation_probability >= 375 && animation_probability < 500){
                ESP_LOGI(TAG, "Bot spying(side watch)\n");
                bot_side_watch_animation();
            }
            else if(animation_probability >= 500 && animation_probability < 625){
                ESP_LOGI(TAG, "Bot talking\n");
                bot_talking_animation();
            }
            else if(animation_probability >= 625 && animation_probability < 750){
                ESP_LOGI(TAG, "Bot moving antena side to side");
                antenna_animation();
            }
            else if(animation_probability >= 750 && animation_probability < 875){
                ESP_LOGI(TAG, "bot is HAPPY");
                bot_happy_animation();  
            }
            else{
                ESP_LOGI(TAG, "Bot is committed Poli[suicide]");
                bot_dead_eyes_animation();
            }
            vTaskDelay(pdMS_TO_TICKS(2000));
        }
    }


/******************************************************************************/
/* End of file ****************************************************************/

