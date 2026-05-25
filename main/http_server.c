#include "esp_http_server.h"
#include "esp_log.h"
#include "freertos/queue.h"
#include "http_server.h"

static const char *TAG = "http_server";

static const char *HTML_PAGE =
"<!DOCTYPE html><html><head>"
"<title>Robot Control</title>"
"<meta name='viewport' content='width=device-width, initial-scale=1'>"
"<style>"
"  body { font-family: sans-serif; text-align: center; background: #1a1a2e; color: white; display: flex; align-items:center; justify-content:center; flex-direction:column;}"
"  h1   { margin-top: 40px; }"
"  button { margin: 10px; padding: 20px 30px; font-size: 18px; "
"           border: none; border-radius: 10px; cursor: pointer; background:blue; color:white;}"
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

static QueueHandle_t s_anim_queue;

/* ── Handler genérico: encola la animación y responde 200 OK ── */
static esp_err_t anim_handler(httpd_req_t *req)
{
    animation_cmd_t cmd = (animation_cmd_t)(intptr_t)req->user_ctx;
    xQueueOverwrite(s_anim_queue, &cmd);   // overwrite: no bloquea nunca
    httpd_resp_sendstr(req, "OK");
    return ESP_OK;
}

/* ── Handler de la página raíz ── */
static esp_err_t root_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_sendstr(req, HTML_PAGE);
    return ESP_OK;
}

/* ── Macro para registrar cada URI limpiamente ── */
#define REGISTER_URI(srv, path, cmd)                        \
    do {                                                    \
        httpd_uri_t _u = {                                  \
            .uri      = (path),                             \
            .method   = HTTP_GET,                           \
            .handler  = anim_handler,                       \
            .user_ctx = (void *)(intptr_t)(cmd),            \
        };                                                  \
        httpd_register_uri_handler((srv), &_u);             \
    } while (0)

void http_server_start(QueueHandle_t anim_queue)
{
    s_anim_queue = anim_queue;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Error arrancando el servidor HTTP");
        return;
    }

    /* Página principal */
    httpd_uri_t root = {
        .uri     = "/",
        .method  = HTTP_GET,
        .handler = root_handler,
    };
    httpd_register_uri_handler(server, &root);

    /* Una URI por animación */
    REGISTER_URI(server, "/idle",    ANIM_IDLE);
    REGISTER_URI(server, "/blink",   ANIM_BLINK);
    REGISTER_URI(server, "/watch",   ANIM_SIDE_WATCH);
    REGISTER_URI(server, "/talk",    ANIM_TALKING);
    REGISTER_URI(server, "/antenna", ANIM_ANTENNA);
    REGISTER_URI(server, "/happy",   ANIM_HAPPY);
    REGISTER_URI(server, "/dead",    ANIM_DEAD);

    ESP_LOGI(TAG, "Servidor HTTP listo");
}