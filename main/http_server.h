#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// The animation commands the web server can request
typedef enum {
    ANIM_IDLE,
    ANIM_BLINK,
    ANIM_SIDE_WATCH,
    ANIM_TALKING,
    ANIM_ANTENNA,
    ANIM_HAPPY,
    ANIM_DEAD
} animation_cmd_t;

// Call this after WiFi connects. Pass the queue that state_controller reads from.
void http_server_start(QueueHandle_t anim_queue);

#endif