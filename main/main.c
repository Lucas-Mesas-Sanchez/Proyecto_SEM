
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include "esp_system.h"
#include "esp_mac.h"

#include "esp_random.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "esp_vfs_dev.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "lcd_crtl.h"
#include "bot_crtl.h"
#include "Wifi.h"
#include "http_server.h"

static QueueHandle_t anim_queue;
#define PROBABILITY_CONTROLL 875
static const char *TAG = "BOT_HAPPY";
TaskHandle_t state_controller_hande = NULL;
static void state_controller(void* param);

void app_main(void)
{
    anim_queue = xQueueCreate(1, sizeof(animation_cmd_t));
    wifi_init();
    ESP_LOGI(__FILE__, "WiFi is being initialized.");
    lcd_crtl_display_init();
    vTaskDelay(pdMS_TO_TICKS(500));
    bot_init();
    
    ESP_LOGI(__FILE__, "Waiting for WiFi...");
    while (!wifi_is_connected()) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    http_server_start(anim_queue);
    ESP_LOGI(__FILE__, "HTTP server is running. Open the IP address shown above in your browser.");
    xTaskCreatePinnedToCore(state_controller,"State_Controller",8192,&anim_queue,2,&state_controller_hande,1);
    
}

static void state_controller(void* param)
{
    
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(100);
    BaseType_t xWasDelayed;
    QueueHandle_t queue = *(QueueHandle_t*)param;
    for(;;)
    {
            animation_cmd_t cmd;
            if (xQueueReceive(queue, &cmd, pdMS_TO_TICKS(10)) == pdTRUE) 
            {
                // A button was pressed on the web page — run the requested animation
                switch (cmd) {

                    case ANIM_IDLE:       
                        bot_idle_animation();
                        ESP_LOGI(__FILE__,"ORDEN RECIBIDA - Idle Animation");         
                    break;

                    case ANIM_BLINK:      
                        bot_blink_animation();
                        ESP_LOGI(__FILE__,"ORDEN RECIBIDA - Blink Animation");        
                    break;

                    case ANIM_SIDE_WATCH: 
                        bot_side_watch_animation();
                        ESP_LOGI(__FILE__,"ORDEN RECIBIDA - Side Watch Animation");     
                    break;

                    case ANIM_TALKING:    
                        bot_talking_animation();
                        ESP_LOGI(__FILE__,"ORDEN RECIBIDA - Talking Animation");      
                    break;

                    case ANIM_ANTENNA:    
                        antenna_animation();
                        ESP_LOGI(__FILE__,"ORDEN RECIBIDA - Antenna Animation");  
                    break;

                    case ANIM_HAPPY:      
                        bot_happy_animation();
                        ESP_LOGI(__FILE__,"ORDEN RECIBIDA - Happy Animation");          
                    break;

                    case ANIM_DEAD:       
                       bot_dead_eyes_animation();
                       ESP_LOGI(__FILE__,"ORDEN RECIBIDA - Dead Animation");      
                    break;

                    default: 
                        break;
                }
            }
            else
            {
                uint32_t animation_probability = (esp_random() % PROBABILITY_CONTROLL);
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
        }
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

}
/******************************************************************************/
/* End of file ****************************************************************/

