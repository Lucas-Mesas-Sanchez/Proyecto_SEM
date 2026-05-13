
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
#define PIXEL_TAM   5
#define OJO_Y       114         
#define OJO_IX      91
#define OJO_DX      131
#define BOCA_X      86
#define BOCA_Y      144
#define BRAZO_Y     169
#define BRAZO_IX    1
#define BRAZO_DX    171
#define ANT_X       86
#define ANT_Y       34
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

static unsigned int bizq_idx = 0;
static unsigned int bder_idx = 0;
static unsigned int antn_idx = 0;
static const sprite_t base           = {base_map, 320, 240};
static const sprite_t boca           = {boca_map, 20, 70};
static const sprite_t ojo_normal     = {ojonormal_map, 20, 20};
static const sprite_t ojoder         = {ojoder_map, 20, 20};
static const sprite_t ojoizq         = {ojoizq_map, 20, 20};
static const sprite_t ojo_cruz       = {ojo_cruz_map, 20, 20};
static const sprite_t antena_base    = {ant_frame1_map, 40, 70};
static const sprite_t brazo_izq_base = {brazo_izq_frame1_map, 60, 80};
static const sprite_t brazo_der_base = {brazo_der_frame1_map, 60, 80};

static const sprite_t animacion_blink[] = {
    {ojonormal_map, 20, 20},
    {blink_frame1_map, 20, 20},
    {blink_frame2_map, 20, 20},
    {blink_frame3_map, 20, 20},
    {blink_frame4_map, 20, 20}
};

static const sprite_t animacion_antena[] = {
    {ant_frame1_map, 40, 70},
    {ant_frame2_map, 40, 70},
    {ant_frame3_map, 40, 70},
    {ant_frame4_map, 40, 70},
    {ant_frame5_map, 40, 70},
    {ant_frame6_map, 40, 70},
    {ant_frame7_map, 40, 70},
};

static const sprite_t animacion_brazo_izq[] = {
    {brazo_izq_frame1_map, 60, 80},
    {brazo_izq_frame2_map, 60, 80}
};


static const sprite_t animacion_brazo_izq[] = {
    {brazo_der_frame1_map, 60, 80},
    {brazo_der_frame2_map, 60, 80}
};
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
    lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
    lcd_canvas_send(&bot_canvas);

    vTaskDelay(pdMS_TO_TICKS(500));
    
    lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, (OJO_Y + PIXEL_TAM));
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, (OJO_Y + PIXEL_TAM));
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, (BOCA_Y + PIXEL_TAM));
    lcd_canvas_send(&bot_canvas);

    vTaskDelay(pdMS_TO_TICKS(500));

    lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
    lcd_canvas_send(&bot_canvas);
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

