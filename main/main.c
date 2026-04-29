
#include <stdio.h>
#include <stdlib.h>
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
#include "pola.c"



TaskHandle_t display_controller_handle = NULL;
TaskHandle_t state_controller_hande = NULL;
static void display_controller(void* param);
static void state_controller(void* param);

canvas_t* out_canvas;
canvas_t canvas1,canvas2;

const sprite_t sprite_anim[] = 
{
    { .width = 240, .height = 320, .data = sprite1_map },
    { .width = 240, .height = 320, .data = sprite2_map },
    { .width = 240, .height = 320, .data = sprite3_map },
    { .width = 240, .height = 320, .data = sprite4_map },
    { .width = 240, .height = 320, .data = sprite5_map },
    { .width = 240, .height = 320, .data = sprite6_map },
    { .width = 240, .height = 320, .data = sprite7_map },
    { .width = 240, .height = 320, .data = sprite8_map },
    { .width = 240, .height = 320, .data = sprite9_map }
    };
void app_main(void)
{
    srand(time(NULL));
    xTaskCreatePinnedToCore(display_controller,"Display_Controller",4098,NULL,2,&display_controller_handle,1);

}

static void display_controller(void* param)
{
    lcd_crtl_canvas_init(&canvas1,sprite1_map,240,320);
    lcd_crtl_canvas_init(&canvas2,sprite1_map,240,320);
    out_canvas = &canvas1;
    lcd_crtl_display_init();
    vTaskDelay(pdMS_TO_TICKS(500));
    
    
    TickType_t xLastTimeWake;
    xLastTimeWake = xTaskGetTickCount(); 
    const TickType_t freq = pdMS_TO_TICKS(120);
    int cont = 0;
	for(;;)
    {
        /* AQUI VA LO QUE QUIERES DIBUJAR */
        lcd_crtl_draw_sprite(out_canvas,&sprite_anim[cont % 9],(uint16_t)0,(uint16_t)0);
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
