#include "ui.h"
#include "../lvgl/lvgl.h"

void crear_mi_interfaz(void) {
	// Crear un botón
	lv_obj_t* btn = lv_btn_create(lv_scr_act());
	lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);

	// Etiqueta
	lv_obj_t* label = lv_label_create(btn);
	lv_label_set_text(label, "Hola desde LVGL!");

	// Si estás en Android, podrías cambiar el texto dinámicamente
#ifdef __ANDROID__
	lv_label_set_text(label, "Hola Android!");
#endif

}