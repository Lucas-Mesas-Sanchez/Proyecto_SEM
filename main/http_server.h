#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Las animaciones que el servidor web puede solicitar
typedef enum {
    ANIM_IDLE,
    ANIM_BLINK,
    ANIM_SIDE_WATCH,
    ANIM_TALKING,
    ANIM_ANTENNA,
    ANIM_HAPPY,
    ANIM_DEAD
} animation_cmd_t;

// Llama a esto después de conectar el WiFi
void http_server_start(QueueHandle_t anim_queue);

#endif // HTTP_SERVER_H