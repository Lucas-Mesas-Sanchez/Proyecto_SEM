# MINIPROYECTO SEM - LCD

Este proyecto implementa un sistema embebido basado en el microcontrolador ESP32 utilizando FreeRTOS para controlar un robot animado en una pantalla TFT (ILI9341 a través de SPI) y un display LCD 16x2 secundario. El comportamiento del robot responde tanto a cambios automáticos de estado basados en probabilidades pseudoaleatorias como a comandos manuales enviados de forma inalámbrica a través de una interfaz web local (Servidor HTTP).

---

## Estructura del Proyecto

El repositorio está organizado en dos carpetas principales, manteniendo todo el código del firmware centralizado de manera lineal y un directorio independiente para las utilidades de conversión de recursos:

### MAIN/
Esta carpeta concentra la totalidad del código fuente, controladores de hardware y configuraciones del firmware del sistema. Todos los archivos de código conviven directamente aquí sin subcarpetas internas:

* **main.c**: Punto de entrada de la aplicación (`app_main`). Se encarga de inicializar el almacenamiento flash (NVS), la conectividad Wi-Fi, los controladores de los displays e iniciar el servidor HTTP. Lanza la tarea de FreeRTOS `state_controller` fijada en el Core 1, la cual gestiona la máquina de estados de las animaciones mediante colas (`QueueHandle_t`) o mediante un generador pseudoaleatorio en ausencia de peticiones externas.
* **bot_crtl.c**: Define las posiciones físicas en los ejes X/Y de cada elemento del robot (ojos, boca, brazos, antena). Implementa las funciones de inicialización y las rutinas de animación cíclicas (`bot_idle_animation`, `bot_blink_animation`, `bot_side_watch_animation`, `bot_talking_animation`, `bot_happy_animation`). Utiliza un buffer de texto sincronizado para enviar estados o frases a una pantalla secundaria.
* **bot_sprites.h**: Archivo de cabecera que almacena los mapas de bits (`_map[]`) codificados en formato RGB565. Contiene los frames secuenciales para simular el parpadeo de ojos, el movimiento de la antena, los cambios de batería en la pantalla del robot y las posiciones de sus extremidades.
* **lcd_crtl.c**: Configura el bus SPI dinámico (SPI2_HOST) mediante la API `esp_lcd` para el controlador TFT ILI9341, administrando pines clave (CS, RST, DC, SCLK, MOSI). Integra además la inicialización por I2C de una pantalla LCD secundaria de 16x2 (dirección `0x27`). Implementa transferencias eficientes por DMA asistidas por semáforos binarios y copias directas a la memoria de video.
* **lcd_crtl.h**: Define las estructuras de datos fundamentales: `sprite_t` (que encapsula dimensiones e imagen estática) y `canvas_t` (que opera como el Framebuffer en la PSRAM o memoria interna). Declara las funciones clave `lcd_crtl_draw_sprite` (que realiza el blitting ignorando el color Magenta como canal de transparencia) y `lcd_crtl_canvas_send`.
* **Wifi.c**: Configura el driver de Wi-Fi del ESP32 en modo Estación (STA). Inicializa la pila TCP/IP, gestiona el bucle de eventos del sistema para reconexiones automáticas tras pérdidas de señal y levanta una tarea de monitorización activa.
* **Wifi_credentials.h**: Archivo de configuración confidencial que define las constantes globales de red `WIFI_SSID` y `WIFI_PASS`.
* **http_server.c**: Inicia el servidor web ligero de `esp_http_server`. Sirve una interfaz HTML interactiva optimizada para dispositivos móviles con botones estilizados. Cada botón ejecuta una petición asíncrona (`fetch('/ruta')`) que se intercepta mediante un handler genérico URI para enviar comandos de animación (`ANIM_BLINK`, `ANIM_TALKING`, etc.) directamente a la cola de FreeRTOS del controlador principal.

### Image2-16BIT-CONVERTER/
Esta carpeta está destinada a la herramienta externa utilizada para el procesamiento y preparación de los recursos gráficos antes de ser integrados en el código embebido:

* **converter.py**: Script en Python basado en la librería Pillow (PIL). Permite transformar cualquier imagen convencional del ordenador a arrays de código en lenguaje C compatibles con el proyecto. Ofrece modos de redimensionado dinámico ("estirar", "recortar" o "barras") y realiza la cuantización a formato **RGB565**, invirtiendo el orden de los bytes (*Byte Swap*) de forma automática para que el driver SPI del ILI9341 procese y dibuje los colores de forma correcta. Reemplaza los canales transparentes por Magenta puro (`0xF81F`) para permitir el solapamiento de capas y transparencias en el microcontrolador.
