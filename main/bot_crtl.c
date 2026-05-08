
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
//ojo izquierdo 91 90,derecho 131 90 tamaño de 20x20. boca 86 120. alto 20 ancho 155-86.
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NUM_PHRASES 9

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

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
static canvas_t bot_canvas;
/* la estructura de los sprites depende de lo que se quiera hacer con ellos. Si va a ser un sprite estatico que se mueve. se declara normal
    sprite_t sprite = {vector_sprite(alocado en bot_sprites.h),altura,ancho}
    si es una animación como por ejemplo la antena recomiendo hacer un vector de sprites. como es mucho copiar y pegar pideselo a la ia que lo haga.
    el sprite que se dibujará siempre antes que cualquiera será el del robot base, sin extremidades ni ojos. por encima se dibujarán el resto.
    Para añadir un sprite usa el convertidor del github. lo pones en una carpeta. pones la imagen que quieres convertir y tienes que poner por consola
    converter.py [foto.extension] <tipo de cuadro (estirado recortado y no se que mas)> <ancho> <alto> o algo así. está en el codigo tmb.

*/
/* Private function prototypes -----------------------------------------------*/
static const sprite_t* next_srpite(const sprite_t* animation,uint8_t* actual,uint8_t size);
static void write_phrase(const char* phrase,uint16_t write_speed);
/* Exported functions --------------------------------------------------------*/
void bot_init(void)
{
    static uint16_t cdata[] = {0};
    lcd_crtl_canvas_init(&bot_canvas,cdata,240,320);
}
void bot_idle_animation()
{
    
}
void bot_blink_animation()
{
   
}
void bot_side_watch_animation()
{

}
void bot_talking_animation()
{
        uint8_t phrase_select = (rand() % NUM_PHRASES);
        const char* phrase = phrases_table[phrase_select];
        LCD_clearScreen();
        LCD_setCursor(0,0);
        if (phrase_select == 2) // Caso especial donde una frase se repite. Como no ocurre más veces no merece la pena hacer un enum
        {
            write_phrase(phrase,100);
            LCD_setCursor(0,1);
        }
            write_phrase(phrase,100);
        //Aqui pones la boca en su posicion original    
}
void bot_moving_anthena_animation()
{

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

