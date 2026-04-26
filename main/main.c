
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
sprite_t bckgorund = {
    .width = 240,
    .height = 320,
    .data = cara_fondo_prueba_map
};
sprite_t ojo =
{
    .width = 30,
    .height = 45,
    .data = ojo_prueba_map
};
void app_main(void)
{
    srand(time(NULL));
    lcd_crtl_canvas_init(&canvas1,cara_fondo_prueba_map,240,320);
    lcd_crtl_canvas_init(&canvas2,cara_fondo_prueba_map,240,320);
    out_canvas = &canvas1;
    lcd_crtl_display_init();
    vTaskDelay(pdMS_TO_TICKS(500));
    
    //xTaskCreatePinnedToCore(display_controller,"Display_Controller",4098,NULL,2,&display_controller_handle,1);
	for(;;)
    {
        int minx = 0, maxx = 210;
        int aleatoriox = (rand() % (maxx - minx + 1)) + minx;

        int miny = 0, maxy = 320-45;
        int aleatorioy = (rand() % (maxy - miny + 1)) + miny;
        lcd_crtl_canvas_clean(out_canvas,0xFFFF);
        lcd_crtl_draw_sprite(out_canvas,&bckgorund,(uint16_t)0,(uint16_t)0);
        lcd_crtl_draw_sprite(out_canvas,&ojo,(uint16_t)aleatoriox,(uint16_t)aleatorioy);
        lcd_crtl_canvas_send(out_canvas);
        //buffer swap
        if (out_canvas == &canvas1)
        {
            out_canvas = &canvas2;
        }
        else
        {
            out_canvas = &canvas1;
        }
    }
}

static void display_controller(void* param)
{
    while (1)
	{
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}
