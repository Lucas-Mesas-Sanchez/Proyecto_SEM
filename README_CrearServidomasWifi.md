# Robot Pet — WiFi Web Controller

This project runs an animated robot face on an ILI9341 SPI display connected to an ESP32-S3.  
It connects to a WiFi access point and hosts a small web page.  
When you open that web page on your phone or laptop and press a button, the robot plays the matching animation.

---

## Table of Contents

1. [How it works — the big picture](#1-how-it-works--the-big-picture)
2. [Hardware required](#2-hardware-required)
3. [Software required](#3-software-required)
4. [Project file structure](#4-project-file-structure)
5. [Step 1 — Set your WiFi credentials](#5-step-1--set-your-wifi-credentials)
6. [Step 2 — Fix CMakeLists.txt](#6-step-2--fix-cmakeliststxt)
7. [Step 3 — Complete http\_server.c](#7-step-3--complete-http_serverc)
8. [Step 4 — Modify main.c](#8-step-4--modify-mainc)
9. [Step 5 — Build and flash](#9-step-5--build-and-flash)
10. [Step 6 — Open the web page](#10-step-6--open-the-web-page)
11. [Troubleshooting](#11-troubleshooting)
12. [Concept glossary](#12-concept-glossary)

---

## 1. How it works — the big picture

```
Your phone/laptop (browser)
        |
        |  1. You open http://192.168.X.X in the browser
        |  2. Browser loads the control page (served by the ESP32)
        |  3. You press "Happy"
        |  4. Browser sends: GET /happy  to the ESP32
        |
        v
     ESP32-S3
        |
        |-- WiFi driver      connects to your router (already done in wifi.c)
        |-- HTTP Server      receives the /happy request
        |-- FreeRTOS Queue   HTTP handler drops the command "ANIM_HAPPY" in
        |-- state_controller reads "ANIM_HAPPY" from queue, calls bot_happy_animation()
        |-- ILI9341 display  shows the happy face
```

The key insight is the **queue**. Without it, the HTTP server and the animation task
would both try to draw to the display at the same time, which would crash or corrupt
the image. The queue forces them to take turns — the HTTP handler writes, the animation
task reads, and only the animation task ever touches the display.

---

## 2. Hardware required

| Part | Details |
|------|---------|
| ESP32-S3 development board | Any variant with at least 4 MB flash |
| ILI9341 SPI display | 240×320, 16-bit colour |
| USB cable | For flashing and serial monitor |
| WiFi access point | A standard home router or phone hotspot |

Your phone and the ESP32 must be connected to **the same WiFi network**.

---

## 3. Software required

| Tool | Version | Purpose |
|------|---------|---------|
| ESP-IDF | v5.x recommended | The ESP32 development framework |
| Python 3 | Any modern version | Required by ESP-IDF build system |
| A terminal | bash / zsh / PowerShell | Running build commands |
| A web browser | Any | Opening the control page |

Install ESP-IDF by following the official guide:  
https://docs.espressif.com/projects/esp-idf/en/stable/esp32s3/get-started/

---

## 4. Project file structure

```
Proyecto_SEM/
├── CMakeLists.txt          ← top-level build file (do not edit)
├── main/
│   ├── CMakeLists.txt      ← lists source files and dependencies  ← YOU EDIT THIS
│   ├── main.c              ← entry point, state_controller task    ← YOU EDIT THIS
│   │
│   ├── wifi.c              ← connects ESP32 to your router
│   ├── wifi.h              ← declares wifi_init() and wifi_is_connected()
│   ├── wifi_credentials.h  ← your SSID and password                ← YOU EDIT THIS
│   │
│   ├── http_server.c       ← the web server and button handlers     ← YOU COMPLETE THIS
│   ├── http_server.h       ← declares http_server_start() and animation_cmd_t
│   │
│   ├── bot_crtl.c          ← animation logic
│   ├── bot_crtl.h          ← declares all bot_*_animation() functions
│   │
│   ├── lcd_crtl.c          ← low-level display drawing
│   └── lcd_crtl.h          ← declares lcd_crtl_display_init() etc.
```

Files marked **YOU EDIT THIS** or **YOU COMPLETE THIS** are the ones touched in this guide.  
All other files already work — do not modify them.

---

## 5. Step 1 — Set your WiFi credentials

Open `main/wifi_credentials.h` and replace the university credentials with your own:

```c
// BEFORE (UPV lab network):
#define WIFI_SSID "UPV-PSK"
#define WIFI_PASS "giirob-pr2-2023"

// AFTER (your network):
#define WIFI_SSID "MyHomeWiFi"
#define WIFI_PASS "MyPassword123"
```

**Rules for credentials:**
- The SSID is the name of the network exactly as it appears when you scan for WiFi on your phone.  
  It is case-sensitive. `"MyWiFi"` and `"mywifi"` are different networks.
- The password is also case-sensitive.
- Keep the double quotes — they are part of the C string syntax.
- Do not commit this file to a public repository with real credentials in it.

**If you want to use your phone as a hotspot:**
- Enable the hotspot on your phone
- Use the hotspot name and password here
- Connect your laptop to the same hotspot so you can open the web page

---

## 6. Step 2 — Fix CMakeLists.txt

Open `main/CMakeLists.txt`. It currently looks like this:

```cmake
idf_component_register(SRCS "bot_crtl.c" "lcd_crtl.c" "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_lcd esp_timer)
```

There are two problems:
1. `wifi.c` and `http_server.c` are not listed in `SRCS` — the build system does not know they exist.
2. The IDF components those files need (`esp_wifi`, `esp_netif`, `nvs_flash`, `esp_http_server`) are not listed in `REQUIRES`.

Replace the entire file content with this:

```cmake
idf_component_register(SRCS "bot_crtl.c" "lcd_crtl.c" "main.c" "wifi.c" "http_server.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_lcd esp_timer esp_wifi esp_netif nvs_flash esp_http_server)
```

**What each new entry does:**

| Entry | Type | Why it is needed |
|-------|------|-----------------|
| `"wifi.c"` | SRCS | Tells CMake to compile this source file |
| `"http_server.c"` | SRCS | Tells CMake to compile this source file |
| `esp_wifi` | REQUIRES | Provides `esp_wifi_*` functions used in wifi.c |
| `esp_netif` | REQUIRES | Provides the network interface layer used in wifi.c |
| `nvs_flash` | REQUIRES | Provides flash storage — WiFi driver requires it |
| `esp_http_server` | REQUIRES | Provides `httpd_*` functions used in http_server.c |

---

## 7. Step 3 — Complete http\_server.c

The file `main/http_server.c` already has the HTML page string and the queue variable declared.  
You need to add the handler functions, the URI descriptors, and the server start function.

Open `main/http_server.c` and add the following **below the existing content** in this exact order.

### 7a. One handler function per animation

A handler is a normal C function that the HTTP server calls automatically when your browser
requests a specific URL. Every handler does three things:

1. Puts the animation command into the queue
2. Sends a short reply to the browser so the request does not hang
3. Returns `ESP_OK` to tell the server everything went fine

```c
static esp_err_t handler_root(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, HTML_PAGE, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t handler_idle(httpd_req_t *req)
{
    animation_cmd_t cmd = ANIM_IDLE;
    xQueueOverwrite(s_anim_queue, &cmd);
    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}

static esp_err_t handler_blink(httpd_req_t *req)
{
    animation_cmd_t cmd = ANIM_BLINK;
    xQueueOverwrite(s_anim_queue, &cmd);
    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}

static esp_err_t handler_watch(httpd_req_t *req)
{
    animation_cmd_t cmd = ANIM_SIDE_WATCH;
    xQueueOverwrite(s_anim_queue, &cmd);
    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}

static esp_err_t handler_talk(httpd_req_t *req)
{
    animation_cmd_t cmd = ANIM_TALKING;
    xQueueOverwrite(s_anim_queue, &cmd);
    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}

static esp_err_t handler_antenna(httpd_req_t *req)
{
    animation_cmd_t cmd = ANIM_ANTENNA;
    xQueueOverwrite(s_anim_queue, &cmd);
    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}

static esp_err_t handler_happy(httpd_req_t *req)
{
    animation_cmd_t cmd = ANIM_HAPPY;
    xQueueOverwrite(s_anim_queue, &cmd);
    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}

static esp_err_t handler_dead(httpd_req_t *req)
{
    animation_cmd_t cmd = ANIM_DEAD;
    xQueueOverwrite(s_anim_queue, &cmd);
    httpd_resp_send(req, "OK", 2);
    return ESP_OK;
}
```

**Why `xQueueOverwrite` and not `xQueueSend`?**  
The queue holds only 1 item. If the user presses "Happy" and then immediately "Blink",
`xQueueOverwrite` replaces "Happy" with "Blink" so the robot always does the latest command.
`xQueueSend` would fail silently if the queue is already full.

### 7b. URI descriptor structs

Each `httpd_uri_t` struct connects a URL path to a handler function.
Add one for every route:

```c
static const httpd_uri_t uri_root     = { .uri = "/",       .method = HTTP_GET, .handler = handler_root     };
static const httpd_uri_t uri_idle     = { .uri = "/idle",   .method = HTTP_GET, .handler = handler_idle     };
static const httpd_uri_t uri_blink    = { .uri = "/blink",  .method = HTTP_GET, .handler = handler_blink    };
static const httpd_uri_t uri_watch    = { .uri = "/watch",  .method = HTTP_GET, .handler = handler_watch    };
static const httpd_uri_t uri_talk     = { .uri = "/talk",   .method = HTTP_GET, .handler = handler_talk     };
static const httpd_uri_t uri_antenna  = { .uri = "/antenna",.method = HTTP_GET, .handler = handler_antenna  };
static const httpd_uri_t uri_happy    = { .uri = "/happy",  .method = HTTP_GET, .handler = handler_happy    };
static const httpd_uri_t uri_dead     = { .uri = "/dead",   .method = HTTP_GET, .handler = handler_dead     };
```

**Why must these come after the handler functions?**  
In C, you cannot reference something before it is declared. The struct references
`handler_happy` by name — so `handler_happy` must appear first in the file.

### 7c. The `http_server_start()` function

This is the function called from `main.c` after WiFi connects.

```c
void http_server_start(QueueHandle_t anim_queue)
{
    // Save the queue so all handlers can use it via s_anim_queue
    s_anim_queue = anim_queue;

    // Configure the server — HTTPD_DEFAULT_CONFIG() fills in all the defaults
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;  // port 80 = standard HTTP, no need to type :80 in the browser

    // Start the server — server is a handle (pointer) to the running server instance
    httpd_handle_t server = NULL;
    esp_err_t err = httpd_start(&server, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server: %s", esp_err_to_name(err));
        return;
    }

    ESP_LOGI(TAG, "HTTP server started on port %d", config.server_port);

    // Register every route — the server now knows which function handles which URL
    httpd_register_uri_handler(server, &uri_root);
    httpd_register_uri_handler(server, &uri_idle);
    httpd_register_uri_handler(server, &uri_blink);
    httpd_register_uri_handler(server, &uri_watch);
    httpd_register_uri_handler(server, &uri_talk);
    httpd_register_uri_handler(server, &uri_antenna);
    httpd_register_uri_handler(server, &uri_happy);
    httpd_register_uri_handler(server, &uri_dead);
}
```

### 7d. Final structure of http\_server.c (top to bottom)

```
1. #include statements
2. static const char *TAG
3. static const char *HTML_PAGE   ← already in the file
4. static QueueHandle_t s_anim_queue  ← already in the file
5. handler_root()
6. handler_idle()
7. handler_blink()
8. handler_watch()
9. handler_talk()
10. handler_antenna()
11. handler_happy()
12. handler_dead()
13. httpd_uri_t structs (one per route)
14. http_server_start()
```

---

## 8. Step 4 — Modify main.c

Three changes are needed in `main/main.c`.

### 8a. Add the http\_server.h include

At the top of the file, alongside the other includes, add:

```c
#include "http_server.h"
```

### 8b. Create the queue and add the animation\_cmd\_t enum

Below the `TAG` definition and before `app_main`, declare the queue handle:

```c
static QueueHandle_t anim_queue;
```

### 8c. Start the server in `app_main` after WiFi connects

Currently `app_main` calls `wifi_init()` and immediately continues.
You need to wait until the connection succeeds before starting the HTTP server.

Replace the `app_main` body with this:

```c
void app_main(void)
{
    srand(time(NULL));

    // Create the queue BEFORE starting any task that uses it
    // 1    = max 1 item in the queue at a time
    // sizeof(animation_cmd_t) = size of each item
    anim_queue = xQueueCreate(1, sizeof(animation_cmd_t));

    wifi_init();
    ESP_LOGI(__FILE__, "WiFi is being initialized.");
    lcd_crtl_display_init();
    vTaskDelay(pdMS_TO_TICKS(500));
    bot_init();

    // Create the animation task, passing the queue so it can receive commands
    xTaskCreatePinnedToCore(state_controller, "State_Controller", 4098,
                            &anim_queue, 2, &state_controller_hande, 0);

    // Wait until WiFi is connected before starting the HTTP server
    ESP_LOGI(__FILE__, "Waiting for WiFi...");
    while (!wifi_is_connected()) {
        vTaskDelay(pdMS_TO_TICKS(500));
    }

    // WiFi is connected — the serial monitor will show the IP address
    // Start the HTTP server and give it the queue
    http_server_start(anim_queue);
    ESP_LOGI(__FILE__, "HTTP server is running. Open the IP address shown above in your browser.");

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
```

### 8d. Update `state_controller` to read from the queue

The task currently picks a random animation every loop.  
Change it so it first checks if the web server sent a command.
If yes, run that. If no, fall back to random behaviour.

The task signature must accept the queue parameter:

```c
static void state_controller(void* param)
{
    QueueHandle_t queue = *(QueueHandle_t*)param;  // unpack the queue from param

    for (;;) {
        animation_cmd_t cmd;

        // xQueueReceive with timeout 0 = "check the queue but do not wait"
        // pdTRUE means a command was available
        if (xQueueReceive(queue, &cmd, 0) == pdTRUE) {
            // A button was pressed on the web page — run the requested animation
            switch (cmd) {
                case ANIM_IDLE:       bot_idle_animation();           break;
                case ANIM_BLINK:      bot_blink_animation();          break;
                case ANIM_SIDE_WATCH: bot_side_watch_animation();     break;
                case ANIM_TALKING:    bot_talking_animation();         break;
                case ANIM_ANTENNA:    bot_moving_anthena_animation();  break;
                case ANIM_HAPPY:      bot_happy_animation();          break;
                case ANIM_DEAD:       bot_dead_eyes_animation();      break;
            }
        } else {
            // No web command — run a random animation as before
            uint16_t animation_probability = (rand() % PROBABILITY_CONTROLL);
            if      (animation_probability < 250)  { bot_idle_animation();          }
            else if (animation_probability < 375)  { bot_blink_animation();         }
            else if (animation_probability < 500)  { bot_side_watch_animation();    }
            else if (animation_probability < 625)  { bot_talking_animation();       }
            else if (animation_probability < 750)  { bot_moving_anthena_animation(); }
            else if (animation_probability < 875)  { bot_happy_animation();         }
            else                                   { bot_dead_eyes_animation();     }
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}
```

---

## 9. Step 5 — Build and flash

Open a terminal in the `Proyecto_SEM` directory.

**Step 9a — Set up the ESP-IDF environment**

Every new terminal session needs this command first.  
It adds the `idf.py` tool to your PATH.

```bash
# On macOS / Linux:
. $HOME/esp/esp-idf/export.sh

# On Windows (Command Prompt):
%USERPROFILE%\esp\esp-idf\export.bat
```

The exact path depends on where you installed ESP-IDF.

**Step 9b — Set the target chip**

Only needed once per project:

```bash
idf.py set-target esp32s3
```

**Step 9c — Build the project**

```bash
idf.py build
```

This compiles all source files and links them into a firmware binary.  
It takes 1–3 minutes the first time. Subsequent builds are much faster.

If it fails with errors, read the first error message — later errors are usually
consequences of the first one.

Common build errors at this stage:

| Error message | Fix |
|---|---|
| `No such file or directory: 'http_server.c'` | Check that `http_server.c` is listed in `main/CMakeLists.txt` |
| `undefined reference to httpd_start` | Add `esp_http_server` to REQUIRES in `main/CMakeLists.txt` |
| `implicit declaration of function 'xQueueCreate'` | Add `#include "freertos/queue.h"` to main.c |
| `animation_cmd_t undeclared` | Add `#include "http_server.h"` to main.c |

**Step 9d — Flash to the ESP32**

Connect the ESP32 via USB, then:

```bash
idf.py flash
```

If the port is not found automatically:

```bash
# macOS:
idf.py -p /dev/tty.usbserial-XXXX flash

# Linux:
idf.py -p /dev/ttyUSB0 flash

# Windows:
idf.py -p COM3 flash
```

**Step 9e — Open the serial monitor**

```bash
idf.py monitor
```

Or combine flash and monitor in one command:

```bash
idf.py flash monitor
```

Press `Ctrl + ]` to exit the monitor.

---

## 10. Step 6 — Open the web page

After flashing, watch the serial monitor output carefully.  
Within a few seconds of boot you will see a line like this:

```
I (3421) wifi station: got ip:192.168.1.42
```

That IP address is your ESP32's address on the local network.

**On your phone or laptop (connected to the same WiFi network):**

1. Open any web browser (Chrome, Safari, Firefox)
2. In the address bar, type the IP address exactly as shown in the monitor:
   ```
   http://192.168.1.42
   ```
   Do not forget the `http://` prefix — without it some browsers try to search instead.
3. The Robot Animations page loads with 7 buttons.
4. Press any button. The robot plays the animation on the display.

**The IP address changes on every boot** if your router uses DHCP (most routers do).  
Check the serial monitor each time you restart the ESP32.  
To get a fixed IP, you would need to configure a static IP or a DHCP reservation in your router — that is an advanced topic not covered here.

---

## 11. Troubleshooting

### The ESP32 never connects to WiFi

Check the serial monitor for repeated lines like:
```
I (xxxx) wifi station: retry to connect to the AP
```

Causes and fixes:

| Cause | Fix |
|---|---|
| Wrong SSID | Check spelling and case in `wifi_credentials.h` |
| Wrong password | Re-check password in `wifi_credentials.h` |
| 5 GHz network | The ESP32 only supports 2.4 GHz. Switch to 2.4 GHz on your router |
| AP is too far | Move the ESP32 closer to the router |
| WPA3-only network | Change `threshold.authmode` in `wifi.c` to `WIFI_AUTH_WPA3_PSK` |

### The web page does not load

- Confirm your phone/laptop is on **the same WiFi network** as the ESP32.  
  A phone hotspot and a home router are different networks — you cannot use one to reach the other.
- Confirm the IP address matches what the serial monitor shows.
- Try `http://` explicitly — some browsers skip it and send HTTPS instead, which will not work.
- Check that `http_server_start()` was called in `app_main` after WiFi connected.

### The page loads but the button does nothing

- Open your browser's developer tools (F12 in Chrome), go to the Console tab.  
  Look for errors after pressing a button.
- Check the serial monitor — the HTTP server logs every incoming request.
- Confirm `xQueueReceive` is called in `state_controller` and the switch cases match the enum values in `http_server.h`.

### The display shows corrupted graphics when pressing buttons quickly

This means two tasks are drawing to the display at the same time.  
Make sure the display is **only** touched inside `state_controller` — never directly from a handler function.  
Handlers must only write to the queue; never call `bot_*_animation()` directly.

### Build error: `first defined here` for `TAG`

Both `wifi.c` and `main.c` define `static const char *TAG`.  
This is fine — `static` means the variable is private to its file.  
If you see this error it means one of the TAG definitions is missing the `static` keyword.

---

## 12. Concept glossary

| Term | Plain-English explanation |
|------|--------------------------|
| **Station mode (STA)** | The ESP32 acts as a WiFi client — like your phone connecting to a router. The opposite is AP mode where the ESP32 acts as the router itself. |
| **DHCP** | Automatic IP address assignment. Your router hands the ESP32 an address like `192.168.1.42`. The address may be different after each reboot. |
| **HTTP** | The protocol browsers use to request web pages. A URL like `http://192.168.1.42/happy` means: connect to `192.168.1.42`, send a GET request for the path `/happy`. |
| **GET request** | The simplest HTTP request. Used for fetching data or triggering an action. The JavaScript `fetch('/happy')` in the button sends a GET request. |
| **FreeRTOS task** | An independent thread of execution running on the ESP32. Multiple tasks run in parallel — the operating system switches between them very quickly. |
| **FreeRTOS Queue** | A thread-safe buffer for passing data between tasks. One task writes, another reads, and FreeRTOS guarantees they never conflict. |
| **Event Group** | A FreeRTOS tool with individual on/off bits. One task sets a bit, another task blocks until that bit is set. Used in `wifi.c` to signal "connected" or "failed". |
| **NVS** | Non-Volatile Storage. A small area of flash memory that keeps its contents after power-off. The WiFi driver uses it to store calibration data. |
| **lwIP** | Lightweight IP — the TCP/IP networking library built into ESP-IDF. Handles the actual sending and receiving of network packets. |
| **httpd\_handle\_t** | An opaque pointer (handle) representing the running HTTP server. Returned by `httpd_start()` and needed to register routes or stop the server. |
| **httpd\_uri\_t** | A struct that binds a URL path and HTTP method to a C function. Registered with `httpd_register_uri_handler()`. |
| **ESP\_ERROR\_CHECK** | A macro that calls `abort()` if the function returns an error code. Stops execution immediately with a useful log message rather than silently continuing with broken state. |
| **volatile** | Tells the C compiler not to cache a variable in a CPU register. Required for variables shared between tasks or modified in interrupt handlers. |
| **static (file scope)** | When applied to a global variable or function, `static` makes it private to the current `.c` file. Other files cannot see it. |
