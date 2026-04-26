/**
    Copyright (C) 2025 The Sistemas Empotrados subject at UPV
    
    @file    lecd_crtl.c
    @author  Lucas Mesas
    @version V0.1
    @date    2026-04-25
    @brief   This file is used to manage a spi ili9341 display more easily
          
    This library has been created to manage a spi ili9341 display. It's mainly focused
    on the drwaing and animating phase more than making it interactive. Also it doesn't
    manage depth or text labels for now.
*/

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "esp_system.h"

#include "esp_mac.h"

#include "esp_lcd_ili9341.h"

#include "esp_lcd_panel_commands.h"
#include "esp_lcd_panel_dev.h"
#include "esp_lcd_panel_interface.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "driver/gpio.h"
#include "esp_vfs_dev.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "lcd_crtl.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define LCD_HOST SPI2_HOST

#define CS_PIN   GPIO_NUM_7
#define RST_PIN  GPIO_NUM_15
#define DC_PIN   GPIO_NUM_8
#define MOSI_PIN GPIO_NUM_17
#define SCLK_PIN GPIO_NUM_18
#define LED_PIN  GPIO_NUM_3   // Movido del 38/1 para evitar conflictos
#define MISO_PIN GPIO_NUM_16  // (O -1 si no lo usas)

#define LCD_PIXEL_CLOCK_HZ 40*1000*1000 //
#define LCD_CMD_BITS 8
#define LCD_PARAM_BITS 8

#define LCD_H_RES 240
#define LCD_V_RES 320
/* Private variables ---------------------------------------------------------*/
static esp_lcd_panel_io_handle_t io_handle = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;
static SemaphoreHandle_t dma_sem = NULL;
/* Private function prototypes -----------------------------------------------*/
static bool IRAM_ATTR notify_transfer_done(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *smph);
/* Exported functions --------------------------------------------------------*/

void lcd_crtl_display_init(void)
{

    dma_sem = xSemaphoreCreateBinary();
    xSemaphoreGive(dma_sem);
    
   

    
    gpio_set_direction(LED_PIN,GPIO_MODE_OUTPUT);
        gpio_set_level(LED_PIN,1);
        spi_bus_config_t buscfg = {
        .sclk_io_num = SCLK_PIN,
        .mosi_io_num = MOSI_PIN,
        .miso_io_num = MISO_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_H_RES * LCD_V_RES * sizeof(uint16_t), 
    };
        ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));


    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = DC_PIN,
        .cs_gpio_num = CS_PIN,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    // Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));
     const esp_lcd_panel_io_callbacks_t cbs = { // Activamos una funció de callback para poder usar DMA
        .on_color_trans_done = notify_transfer_done,
    };
    ESP_ERROR_CHECK(esp_lcd_panel_io_register_event_callbacks(io_handle, &cbs, dma_sem));
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = RST_PIN,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel = 16,
    };
    // Create LCD panel handle for ili9341, with the SPI IO device handle
    ESP_ERROR_CHECK(esp_lcd_new_panel_ili9341(io_handle, &panel_config, &panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    vTaskDelay(pdMS_TO_TICKS(120));

    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    vTaskDelay(pdMS_TO_TICKS(50));

    ESP_LOGI(__FILE__, "Panel iniciado correctamente");
}

void lcd_crtl_canvas_init(canvas_t* canvas, uint16_t* cdata,uint16_t w,uint16_t h)
{
    canvas->width = w;
    canvas->height = h;
    

    canvas->data = (uint16_t *)heap_caps_malloc(w * h * sizeof(uint16_t), MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL); 
    memcpy(canvas->data,cdata,w * h * sizeof(uint16_t));
}
bool lcd_crtl_draw_sprite(canvas_t* canvas, sprite_t* sprite, uint16_t x0, uint16_t y0)
{
    bool err = true;
    if (((sprite->width + x0) > canvas->width ) || ((sprite->height + y0) > canvas->height))
    {
        err = false;
    }
    if (err)
    {
        for(uint16_t y = 0; y < sprite->height; y++)
        {
            for (uint16_t x = 0; x < sprite->width; x++)
            {
                uint16_t canvas_idx = (y0 + y) * canvas->width + (x0 + x);
                uint16_t sprite_idx = y * sprite->width + x;
                if (sprite->data[sprite_idx] != LCD_CRTL_TRANSPARENT_COLOR) //Color auxiliar que permite hacer formas no cuadradas. En este caso magenta
                {
                    canvas->data[canvas_idx] = sprite->data[sprite_idx];
                }
            }
        }
    }
    return err;
}

void lcd_crtl_canvas_clean(canvas_t* canvas, uint16_t color)
{
    for(uint16_t y = 0; y < canvas->height; y++)
        {
            for (uint16_t x = 0; x < canvas->width; x++)
            {
                uint16_t canvas_idx = y * canvas->width + x;
                canvas->data[canvas_idx] = color;
            }
        }
}

void lcd_crtl_canvas_send(canvas_t* canvas)
{
    xSemaphoreTake(dma_sem, portMAX_DELAY);
    esp_lcd_panel_draw_bitmap(panel_handle,0,0,canvas->width,canvas->height,canvas->data);
}


/* Private functions ---------------------------------------------------------*/
static bool IRAM_ATTR notify_transfer_done(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata, void *smph)
{
    BaseType_t high_task_wakeup = pdFALSE;
    
    xSemaphoreGiveFromISR((SemaphoreHandle_t)smph, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}



/* End of file ****************************************************************/

