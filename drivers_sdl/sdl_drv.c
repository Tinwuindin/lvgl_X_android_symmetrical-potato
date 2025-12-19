#include "sdl_drv.h"
#include <SDL.h>
#include <stdlib.h>
#include "../lvgl/lvgl.h"

/* --- CONFIGURACIÓN --- */
#define SDL_MAIN_HANDLED /* Para evitar conflictos con el main de SDL */

/* Resolución por defecto para PC (en Android se ignorará y usará fullscreen) */
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 480

/* Objetos de SDL */
static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;

/* Buffer de dibujo para LVGL (1/10 de la pantalla es un tamaño típico recomendado) */
#define BUF_SIZE (WINDOW_WIDTH * WINDOW_HEIGHT / 10)
static lv_draw_buf_t disp_buf;
static lv_color_t buf1[BUF_SIZE];
/* Si quieres doble buffer, descomenta esto y añádelo abajo:
   static lv_color_t buf2[BUF_SIZE]; */

   /* --- 1. FUNCIÓN DE DIBUJO (FLUSH) ---
	  LVGL llama a esto cuando termina de renderizar una parte de la pantalla
	  y quiere que la mostremos en el hardware real (SDL). */
static void monitor_flush(lv_fs_drv_t* disp_drv, const lv_area_t* area, lv_color_t* color_p)
{
	SDL_Rect r;
	r.x = area->x1;
	r.y = area->y1;
	r.w = area->x2 - area->x1 + 1;
	r.h = area->y2 - area->y1 + 1;

	/* Actualizamos la textura de SDL con los píxeles calculados por LVGL */
	SDL_UpdateTexture(texture, &r, color_p, r.w * sizeof(lv_color_t));

	/* Dibujamos la textura en la ventana */
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);

	/* IMPORTANTE: Informar a LVGL que estamos listos */
	lv_disp_flush_ready(disp_drv);
}

/* --- 2. FUNCIÓN DE ENTRADA (MOUSE/TOUCH) ---
   LVGL llama a esto periódicamente para saber dónde está el puntero/dedo. */
static void mouse_read(lv_indev_drv_t* indev_drv, lv_indev_data_t* data)
{
	int x, y;

	/* SDL maneja Touch y Mouse con la misma función básica para un solo punto */
	Uint32 buttons = SDL_GetMouseState(&x, &y);

	if (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		data->state = LV_INDEV_STATE_PRESSED;
	}
	else {
		data->state = LV_INDEV_STATE_RELEASED;
	}

	data->point.x = (lv_coord_t)x;
	data->point.y = (lv_coord_t)y;
}

/* --- 3. INICIALIZACIÓN PRINCIPAL --- */
void sdl_hal_init(void) {
	/* Inicializar SDL Video */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		// Manejar error (printf, log, etc)
		return;
	}

	int width = WINDOW_WIDTH;
	int height = WINDOW_HEIGHT;

	/* Configuración específica según la plataforma */
#ifdef __ANDROID__
	/* En Android queremos pantalla completa real */
	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) == 0) {
		width = dm.w;
		height = dm.h;
	}

	window = SDL_CreateWindow("LVGL Android",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height,
		SDL_WINDOW_FULLSCREEN); // Fullscreen nativo
#else
	/* En PC creamos una ventana redimensionable o fija */z
	window = SDL_CreateWindow("LVGL Simulator PC",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height,
		0); /* Flags: SDL_WINDOW_RESIZABLE si quieres */
#endif

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

	/* Textura donde LVGL pintará. Formato RGB565 es rápido y ligero */
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB565,
		SDL_TEXTUREACCESS_STREAMING,
		width, height);

	/* --- REGISTRO EN LVGL --- */

	/* 1. Inicializar buffer de dibujo */
	lv_disp_draw_buf_init(&disp_buf, buf1, NULL, BUF_SIZE);

	/* 2. Registrar el driver de pantalla */
	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);

	disp_drv.draw_buf = &disp_buf;
	disp_drv.flush_cb = monitor_flush;
	disp_drv.hor_res = width;
	disp_drv.ver_res = height;

	lv_disp_drv_register(&disp_drv);

	/* 3. Registrar el mouse / touch */
	static lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);

	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = mouse_read;

	lv_indev_drv_register(&indev_drv);
}