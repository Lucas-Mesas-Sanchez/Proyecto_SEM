
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
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NUM_PHRASES 9
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static const sprite_t* next_srpite(const sprite_t* animation,uint8_t* actual,uint8_t size);
static void write_phrase(const char* phrase,uint16_t write_speed);
/* Exported functions --------------------------------------------------------*/
void bot_idle_animation(canvas_t* canvas);
void bot_blink_animation(canvas_t* canvas);
void bot_side_watch_animation(canvas_t* canvas);
void bot_talking_animation(canvas_t* canvas);
void bot_moving_anthena_animation(canvas_t* canvas);
void bot_dead_eyes_animation(canvas_t* canvas);
void bot_happy_animation(canvas_t* canvas);

/******************************************************************************/
/* End of file ****************************************************************/


