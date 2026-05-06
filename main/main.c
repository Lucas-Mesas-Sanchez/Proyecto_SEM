
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
/* Private define ------------------------------------------------------------*/
#define PROBABILITY_CONTROLL 1000
/* Private macro -------------------------------------------------------------*/
static const char *TAG = "BOT_HAPPY"
/* Private variables ---------------------------------------------------------*/
TaskHandle_t display_controller_handle = NULL;
TaskHandle_t state_controller_hande = NULL;
canvas_t* out_canvas;
canvas_t canvas1,canvas2;
// idle = 25%, hablar 12.5%, blink=12.5%, mirar_a_los_lados=12.5%, 
// mover_antena = 12.5%, ojos cruz = 12.5% feliz = 12.5%
static uint16_t animation_propability; // rango de valores de 0 a 999 


/* Private function prototypes -----------------------------------------------*/
static void display_controller(void* param);
static void state_controller(void* param);

/* Private functions ---------------------------------------------------------*/


void app_main(void)
{
    srand(time(NULL));
    xTaskCreatePinnedToCore(display_controller,"Display_Controller",4098,NULL,2,&display_controller_handle,1);
    xTaskCreatePinnedToCore(state_controller,"State_Controller",4098,NULL,2,&state_controller_hande,1);

}

static void display_controller(void* param)
{
    
    out_canvas = &canvas1;
    lcd_crtl_display_init();
    vTaskDelay(pdMS_TO_TICKS(500));
    
    
    TickType_t xLastTimeWake;
    xLastTimeWake = xTaskGetTickCount(); 
    const TickType_t freq = pdMS_TO_TICKS(83.3333f);
    int cont = 0;
	for(;;)
    {
        /* AQUI VA LO QUE QUIERES DIBUJAR */
        
        /* FUNCION DE ESCRITURA */
        lcd_crtl_canvas_send(out_canvas);
        cont ++;
        /* BUFFER SWAP */
        if (out_canvas == &canvas1)
        {
            out_canvas = &canvas2;
        }
        else
        {
            out_canvas = &canvas1;
        }
        /* DELAY PARA TENER FPS ESTABLES */
        xTaskDelayUntil(&xLastTimeWake,freq);
    }
}
static void state_controller(void* param){
    
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(100f);
    BaseType_t xWasDelayed;

    for(;;){
        if(!bot_animation_is_active){
            animation_propability = (rand() % PROBABILITY_CONTROLL);
            if(animation_propability >= 0 && animation_probability < 250){
                bot_idle_animation();
                ESP_LOGI(TAG, "Bot on Idle");
            }
            else if(animation_propability >= 250 && animation_probability < 375){
                bot_blink_animation();
                ESP_LOGI(TAG, "Bot blinking\n");

            }
            else if(animation_propability >= 375 && animation_probability < 500){
                bot_side_watch_animation();
                ESP_LOGI(TAG, "Bot spying(side watch)\n");

            }
            else if(animation_propability >= 500 && animation_probability < 625){
                bot_talking_animation();
                ESP_LOGI(TAG, "Bot talking\n");

            }
            else if(animation_propability >= 625 && animation_probability < 750){
                bot_moving_anthena_animation();
                ESP_LOGI(TAG, "Bot moving antena side to side");

            }
            else if(animation_propability >= 750 && animation_probability < 875){
                bot_happy_animation();
                ESP_LOGI(TAG, "bot is HAPPY");
            }
            else{
                bot_dead_eyes_animation();
                ESP_LOGI(TAG, "Bot is committed Poli[suicide]");
            }
        }
        else{
            xWasDelayed = xTaskDelayUntil(&xLastWakeTime, xFrequency);
        }

    }

}

