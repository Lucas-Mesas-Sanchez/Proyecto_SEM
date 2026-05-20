#include "esp_http_server.h"   // gives us httpd_* functions
#include "esp_log.h"
#include "freertos/queue.h"
#include "http_server.h"


static const char *TAG = "http_server";

//página web
static const char *HTML_PAGE =
"<!DOCTYPE html><html><head>"
"<title>Robot Control</title>"
"<meta name='viewport' content='width=device-width, initial-scale=1'>"
"<style>"
"  body { font-family: sans-serif; text-align: center; background: #1a1a2e; color: white; display: flex; align-ites:center; justify-content:center;}"
"  h1   { margin-top: 40px; }"
"  button { margin: 10px; padding: 20px 30px; font-size: 18px; "
"           border: none; border-radius: 10px; cursor: pointer; background:blue;}"
"</style></head><body>"
"<h1>Robot Animations</h1>"
"<button onclick=\"fetch('/idle')\">Idle</button>"
"<button onclick=\"fetch('/blink')\">Blink</button>"
"<button onclick=\"fetch('/watch')\">Side Watch</button>"
"<button onclick=\"fetch('/talk')\">Talk</button>"
"<button onclick=\"fetch('/antenna')\">Antenna moving</button>"
"<button onclick=\"fetch('/happy')\">Happy</button>"
"<button onclick=\"fetch('/dead')\">Poli[Suicide]</button>"
"</body></html>";


// This variable lives only inside http_server.c
// It is set once when http_server_start() is called
// Every handler function reads from this variable
static QueueHandle_t s_anim_queue;

// Al final de tu archivo http_server.c:
void http_server_start(QueueHandle_t anim_queue) 
{
    // Guardamos la cola global del bot en nuestra variable estática local
    s_anim_queue = anim_queue;

    ESP_LOGI(TAG, "Iniciando el servidor HTTP...");

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Aquí iría el código para registrar tus handlers de URIs (/idle, /blink, etc.)
    // y finalmente arrancar el servidor:
    // if (httpd_start(&server, &config) == ESP_OK) { ... }
}

void http_server_start(QueueHandle_t anim_queue);