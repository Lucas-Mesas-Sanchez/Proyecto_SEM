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
    @brief  Init Function of the bot
*/
void bot_init(void);
/**
    @brief  Activates the idle animation of the bot. NOT CONCURRENT
*/
void bot_idle_animation(void);
/**
    @brief   Activates the blink animation of the bot. NOT CONCURRENT
*/
void bot_blink_animation(void);
/**
    @brief  Activates the watch side to side animation of the bot. NOT CONCURRENT
*/
void bot_side_watch_animation(void);
/**
    @brief  Activates the talk animation of the bot. NOT CONCURRENT
*/
void bot_talking_animation(void);
/**
    @brief  Activates moving the anthena side to side animation of the bot. NOT CONCURRENT
*/
void bot_moving_anthena_animation(void);
/**
    @brief  Activates the happy animation of the bot. NOT CONCURRENT
*/
void bot_happy_animation(void);
/**
    @brief  Activates the dead animation of the bot. NOT CONCURRENT
*/
void bot_dead_eyes_animation(void);
#ifdef __cplusplus
}
#endif

#endif
/*** End of file **************************************************************/

