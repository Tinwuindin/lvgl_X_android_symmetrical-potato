#include "../lvgl/lvgl.h"
#include "../src_shared/ui.h"
#include "../drivers_sdl/sdl_drv.h" // Supongamos que aquí inicializas SDL
#include <SDL2/SDL.h>

int main(int argc, char *argv[]) {
    // 1. Inicializar LVGL
    lv_init();

    // 2. Inicializar Hardware (SDL)
    // Esta función personalizada tuya debe crear la ventana SDL y buffers LVGL
    sdl_hal_init(); 

    // 3. Cargar tu UI Compartida
    crear_mi_interfaz();

    // 4. Bucle infinito
    while(1) {
        lv_timer_handler(); // LVGL gestiona tareas
        SDL_Delay(5);       // No quemar CPU
        
        // En PC necesitamos manejar el evento de "Cerrar ventana"
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) return 0;
            // Pasar eventos de mouse/teclado a LVGL aquí
        }
    }
    return 0;
}