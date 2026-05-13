/**
    Copyright (C) 2025 The Sistemas Empotrados subject at UPV
    
    @file    lcd_crtl.h
    @author  Lucas Mesas Sanchez
    @version V0.1
    @date    2026-04-25
    @brief   This file is used to manage a spi ili9341 display more easily
          
    This library has been created to manage a spi ili9341 display. It's mainly focused
    on the drwaing and animating phase more than making it interactive. Also it doesn't
    manage depth or text labels for now.
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LCD_CRTL_H
#define LCD_CRTL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
typedef struct 
{
    const uint16_t *data;
    uint16_t width;
    uint16_t height;
}sprite_t;

typedef struct 
{
    uint16_t *data;
    uint16_t height;
    uint16_t width;
    size_t   aligned_size;
}canvas_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
#define LCD_CRTL_WHITE 0xFFFF
#define LCD_CRTL_BLACK 0x0000

#define LCD_CRTL_TRANSPARENT_COLOR 0x1ff8
/* Exported functions --------------------------------------------------------*/
/******************************************************************************/
/**
    @brief  Init Function of the display
*/
void lcd_crtl_display_init(void);

/******************************************************************************/
/**
    @brief  Init Function of a canvas. If you have initialized the data of the canvas with DMA in mind you don't need to use this function
    @param canvas pointer to canvas you want to copy the data
    @param cdata pointer to the data you are gonna copy
    @param w width of the canvas
    @param h height of the canvas
*/
void lcd_crtl_canvas_init(canvas_t* canvas, uint16_t* cdata,uint16_t w,uint16_t h);
/**
    @brief  Function to draw a sprite in a canvas. The Magenta color is a transparent color to make non squared sprites
    @param  canvas Pointer to the canvas where you want to draw. Sprite that is gonna be drawn
    @param  sprite Pointer to the sprite you want to draw
    @param  x0 Coordinate x of the canvas where the sprite is gonna be drawn
    @param y0 Coordinate y of the canvas where the sprite is gonna be drawn
    @retval True if the sprite fits in the canvas and False if not
*/
bool lcd_crtl_draw_sprite(canvas_t* canvas,sprite_t* sprite, uint16_t x0, uint16_t y0);

/******************************************************************************/
/******************************************************************************/
/**
    @brief  Function to clear a canvas with a certain color
    @param  canvas Pointer to the canvas you are gonna display
    @param color Clean color of the canvas

*/
void lcd_crtl_canvas_clean(canvas_t* canvas, uint16_t color);

/**
    @brief  Function to send a canvas to de display
    @param  canvas Pointer to the canvas you are gonna display
*/
void lcd_crtl_canvas_send(canvas_t* canvas);
#ifdef __cplusplus
}
#endif

#endif
/*** End of file **************************************************************/

