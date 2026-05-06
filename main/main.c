
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




TaskHandle_t display_controller_handle = NULL;
TaskHandle_t state_controller_hande = NULL;
static void display_controller(void* param);
static void state_controller(void* param);

canvas_t* out_canvas;
canvas_t canvas1,canvas2;


void app_main(void)
{
    srand(time(NULL));
    xTaskCreatePinnedToCore(display_controller,"Display_Controller",4098,NULL,2,&display_controller_handle,1);

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
