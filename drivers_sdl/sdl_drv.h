#ifndef SDL_DRV_H
#define SDL_DRV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../lvgl/lvgl.h"

/**
 * Inicializa SDL, la ventana, el renderer y conecta los buffers de LVGL.
 * Llama a esto justo después de lv_init().
 */
void sdl_hal_init(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SDL_DRV_H */