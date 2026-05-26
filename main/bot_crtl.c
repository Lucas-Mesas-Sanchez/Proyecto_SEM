
/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <string.h>
#include "HD44780.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_random.h"
#include "bot_crtl.h"
#include "lcd_crtl.h"
#include "bot_sprites.h"
//ojo izquierdo 91 90,derecho 131 90 tamaño de 20x20. boca 86 120. alto 20 ancho 155-86.
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NUM_PHRASES 9
#define PIXEL_TAM   5
#define OJO_Y       115         
#define OJO_IX      90
#define OJO_DX      130
#define BOCA_X      85
#define BOCA_Y      145
#define BRAZO_Y     170
#define BRAZO_IX    0
#define BRAZO_DX    160
#define ANT_X       85
#define ANT_Y       35
#define PTLL_X      100
#define PTLL_Y      225
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

static unsigned int bizq_idx = 0;
static unsigned int bder_idx = 0;
static unsigned int antn_idx = 0;
static const sprite_t base         = {base_map, 240, 320};

static const sprite_t boca          = {boca_map, 70, 20};
static const sprite_t ojonormal     = {ojonormal_map, 20, 20};


static const sprite_t ojoder         = {ojoder_map, 20, 20};
static const sprite_t ojoizq         = {ojoizq_map, 20, 20};
static const sprite_t ojo_cruz       = {ojo_cruz_map, 20, 20};


static const sprite_t antena_base    = {ant_frame1_map, 70, 40};


static const sprite_t brazo_izq_base = {brazo_izq_frame1_map, 80, 60};
static const sprite_t brazo_der_base = {brazo_der_frame1_map,80, 60};


static const sprite_t pantalla_base = {pantalla_map, 40, 15};

static const sprite_t animacion_no_batt[] = {
    {pantalla_map, 40, 15},
    {pantalla_pila_alta_map, 40, 15},
    {pantalla_pila_baja_map, 40, 15},
    {pantalla_pila_vacia_map, 40, 15}
};

static const sprite_t animacion_blink[] = {
    {ojonormal_map, 20, 20},
    {blink_frame1_map, 20, 20},
    {blink_frame2_map, 20, 20},
    {blink_frame3_map, 20, 20},
    {blink_frame4_map, 20, 20}
};

static const sprite_t animacion_antena[] = {
    {ant_frame1_map, 70, 40},
    {ant_frame2_map, 70, 40},
    {ant_frame3_map, 70, 40},
    {ant_frame4_map, 70, 40},
    {ant_frame5_map, 70, 40},
    {ant_frame6_map, 70, 40},
    {ant_frame7_map, 70, 40},
};

static const sprite_t animacion_brazo_izq[] = {
    {brazo_izq_frame1_map, 80, 60},
    {brazo_izq_frame2_map, 80, 60}
};


static const sprite_t animacion_brazo_der[] = {
    {brazo_der_frame1_map, 80, 60},
    {brazo_der_frame2_map, 80, 60}
};

/* Private function prototypes -----------------------------------------------*/
static void write_phrase(const char* phrase,uint16_t write_speed);
/* Exported functions --------------------------------------------------------*/
void bot_init(void)
{
    static uint16_t cdata[] = {0};
    lcd_crtl_canvas_init(&bot_canvas,cdata,240,320);
    lcd_crtl_canvas_clean(&bot_canvas,0xffff);
    lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
    lcd_crtl_canvas_send(&bot_canvas);
    
}
void bot_idle_animation()
{
    lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
   
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
    lcd_crtl_canvas_send(&bot_canvas);

    vTaskDelay(pdMS_TO_TICKS(500));
    
    lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, (OJO_Y - PIXEL_TAM));
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, (OJO_Y - PIXEL_TAM));
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, (BOCA_Y - PIXEL_TAM));
    lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
    lcd_crtl_canvas_send(&bot_canvas);

    vTaskDelay(pdMS_TO_TICKS(500));

    lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
    lcd_crtl_canvas_send(&bot_canvas);
}
void bot_blink_animation()
{
   for(uint8_t i = 0; i < 5 ; i++)
   {
        lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
        lcd_crtl_draw_sprite(&bot_canvas, &animacion_blink[i], OJO_IX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &animacion_blink[i], OJO_DX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
        lcd_crtl_canvas_send(&bot_canvas);

        vTaskDelay(pdMS_TO_TICKS(75));
    }
    for(uint8_t i = 4; i > 0; i--)
    {
        lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
        lcd_crtl_draw_sprite(&bot_canvas, &animacion_blink[i], OJO_IX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &animacion_blink[i], OJO_DX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
        lcd_crtl_canvas_send(&bot_canvas);

        vTaskDelay(pdMS_TO_TICKS(75));
    }
    lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
    lcd_crtl_canvas_send(&bot_canvas);
}
void bot_side_watch_animation()
{
    lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
    lcd_crtl_canvas_send(&bot_canvas);

    vTaskDelay(pdMS_TO_TICKS(750));

    lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
    lcd_crtl_draw_sprite(&bot_canvas, &ojoizq, OJO_IX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &ojoizq, OJO_DX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
    lcd_crtl_canvas_send(&bot_canvas);
        
    vTaskDelay(pdMS_TO_TICKS(750));
    
    lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
    lcd_crtl_canvas_send(&bot_canvas);

    vTaskDelay(pdMS_TO_TICKS(750));
    
    lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
    lcd_crtl_draw_sprite(&bot_canvas, &ojoder, OJO_IX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &ojoder, OJO_DX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
    lcd_crtl_canvas_send(&bot_canvas);

    vTaskDelay(pdMS_TO_TICKS(750));
    
    lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
    lcd_crtl_canvas_send(&bot_canvas);
}
void bot_talking_animation()
{
        uint8_t phrase_select = (esp_random() % NUM_PHRASES);
        const char* phrase = phrases_table[phrase_select];
        LCD_clearScreen();
        LCD_setCursor(0,0);
        write_phrase(phrase,75);
        if (phrase_select == 2) // Caso especial donde una frase se repite. Como no ocurre más veces no merece la pena hacer un enum
        {
            LCD_setCursor(0,1);
            write_phrase(phrase,75);
            
        }
        if(phrase_select == 8) // Caso especial donde se ejecuta una animación tras decir la frase.
        {
            bot_dead_eyes_animation();
        }
        vTaskDelay(pdMS_TO_TICKS(500));
        LCD_clearScreen();  
}

void antenna_animation ()
{
    for(uint8_t i = 0; i < 7; i++)
    {
        lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
        lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &animacion_antena[i], ANT_X, ANT_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
        lcd_crtl_canvas_send(&bot_canvas);

        vTaskDelay(pdMS_TO_TICKS(75));
    }
    
    for(uint8_t i = 6; i > 0; i--)
    {
        lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
        lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &animacion_antena[i], ANT_X, ANT_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
        lcd_crtl_canvas_send(&bot_canvas);

        vTaskDelay(pdMS_TO_TICKS(75));
    }
    lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
        lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
        lcd_crtl_canvas_send(&bot_canvas);
}


void bot_dead_eyes_animation()
{
    for(uint8_t i = 0; i < 4; i++)
    {
        lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
        lcd_crtl_draw_sprite(&bot_canvas, &ojo_cruz, OJO_IX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &ojo_cruz, OJO_DX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &animacion_no_batt[i], PTLL_X, PTLL_Y);
        lcd_crtl_canvas_send(&bot_canvas);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
    lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
    lcd_crtl_canvas_send(&bot_canvas);
    
}


void bot_happy_animation()
{
    for (uint8_t i = 0; i < 2; i++)
    {
        lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
        lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &animacion_brazo_izq[i], BRAZO_IX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &animacion_brazo_der[i], BRAZO_DX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
        lcd_crtl_canvas_send(&bot_canvas);
        vTaskDelay(pdMS_TO_TICKS(750));
    }
    lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
    lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
    lcd_crtl_canvas_send(&bot_canvas);
}
/* Private functions ---------------------------------------------------------*/

static void write_phrase(const char* phrase,uint16_t write_speed)
{
    for (uint8_t phrase_ch = 0 ; phrase_ch < strlen(phrase); phrase_ch++ )
    {
       //Aqui mueve la boca ariiba y abajo
        lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
        lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, (BOCA_Y- PIXEL_TAM));
        lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
        lcd_crtl_canvas_send(&bot_canvas);
        LCD_writeChar(phrase[phrase_ch]);
        vTaskDelay(pdMS_TO_TICKS(100));
        lcd_crtl_draw_sprite(&bot_canvas, &base, 0, 0);
        lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_IX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &ojonormal, OJO_DX, OJO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &brazo_izq_base, BRAZO_IX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &antena_base, ANT_X, ANT_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &brazo_der_base, BRAZO_DX, BRAZO_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &boca, BOCA_X, BOCA_Y);
        lcd_crtl_draw_sprite(&bot_canvas, &pantalla_base, PTLL_X, PTLL_Y);
        lcd_crtl_canvas_send(&bot_canvas);
        vTaskDelay(pdMS_TO_TICKS(write_speed));
    }
}