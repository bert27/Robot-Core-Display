#ifndef MY_CARD_H
#define MY_CARD_H

#include "lvgl.h"

/**
 * Crea una tarjeta interactiva con imagen arriba y texto abajo.
 * @param parent Objeto padre
 * @param icon_src Fuente de la imagen (const void* para soportar &img_... o símbolos)
 * @param text Texto a mostrar
 * @param width Ancho del componente
 * @param height Alto del componente
 * @param bg_color Color de fondo
 * @param event_cb Callback de evento (click)
 * @param font Fuente para el texto (opcional, por defecto Montserrat 20 si se usa en contexto global o la del tema)
 * @return Puntero al objeto creado
 */
lv_obj_t* create_custom_card(lv_obj_t* parent, const void* icon_src, const char* text, int32_t width, int32_t height, lv_color_t bg_color, lv_event_cb_t event_cb, const lv_font_t * font);

#endif // MY_CARD_H
