/**
    Copyright (C) 2025 The Sistemas Empotrados subject at UPV
    
    @file    lcd_crtl.h
    @author  Lucas Mesas Sanchez
    @version V0.1
    @date    2026-04-25
    @brief   This file is used to manage a robot pet
          
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BOT_CRTL_H
#define BOT_CRTL_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/******************************************************************************/
/**
    @brief  Init Function of the display
*/
void bot_init(void);
void bot_idle_animation(canvas_t* canvas);
void bot_blink_animation(canvas_t* canvas);
void bot_side_watch_animation(canvas_t* canvas);
void bot_talking_animation(canvas_t* canvas);
void bot_moving_anthena_animation(canvas_t* canvas);
#ifdef __cplusplus
}
#endif

#endif
/*** End of file **************************************************************/

