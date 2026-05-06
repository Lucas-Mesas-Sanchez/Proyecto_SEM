
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <string.h>
#include "HD44780.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bot_crtl.h"
#include "lcd_crtl.h"
#include "bot_sprites.h"
#include "bot_crtl.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NUM_PHRASES 9
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static bool bot_animation_is_active = false;
static const char* phrases_table[] = 
{
    "Hello World !!",
    "Goodbye World !!",
    "Me Seguis?",
    "Connected",
    "Disconnected",
    "Please Charge",
    "Rebollo Dimision",
    "Poli[traumaT]",
    "Me mueeerooooooo"


};
/* Private function prototypes -----------------------------------------------*/
// static const sprite_t* next_srpite(const sprite_t* animation,uint8_t* actual,uint8_t size);
// static void write_phrase(const char* phrase,uint16_t write_speed);
/* Exported functions --------------------------------------------------------*/
void bot_idle_animation(canvas_t* canvas)
{
    
}
void bot_blink_animation(canvas_t* canvas)
{
    bot_animation_is_active = true;

    bot_animation_is_active = false;
}
void bot_side_watch_animation(canvas_t* canvas)
{
    bot_animation_is_active = true;

    bot_animation_is_active = false;
}
void bot_talking_animation(canvas_t* canvas)
{
    bot_animation_is_active = true;
        uint8_t phrase_select = (rand() % NUM_PHRASES);
        const char* phrase;
        strcpy(phrase,phrases_table[phrase_select]);
        LCD_clearScreen();
        LCD_setCursor(0,0);
        if (phrase_select == 2) // Caso especial donde una frase se repite. Como no ocurre más veces no merece la pena hacer un enum
        {
            write_phrase(phrase,100);
            LCD_setCursor(0,1);
        }
            write_phrase(phrase,100);
        //Aqui pones la boca en su posicion original    
    bot_animation_is_active = false;
}
void bot_moving_anthena_animation(canvas_t* canvas)
{
    bot_animation_is_active = true;

    bot_animation_is_active = false;
}
void bot_dead_eyes_animation(canvas_t* canvas){
    bot_animation_is_active = true;

    bot_animation_is_active = false;
}
void bot_happy_animation(canvas_t* canvas){
    bot_animation_is_active = true;

    bot_animation_is_active = false;
}


/* Private functions ---------------------------------------------------------*/
static const sprite_t* next_srpite(const sprite_t* animation,uint8_t* actual,uint8_t size)
{
    *actual = (*actual +1) % size;
    return &animation[*actual];
}
static void write_phrase(const char* phrase,uint16_t write_speed)
{
    for (uint8_t phrase_ch = 0 ; phrase_ch < strlen(phrase); phrase_ch++ )
    {
       //Aqui mueve la boca ariiba y abajo
       LCD_writeChar(phrase[phrase_ch]);
       vTaskDelay(pdMS_TO_TICKS(write_speed));
    }
}
/******************************************************************************/
/* End of file ****************************************************************/

