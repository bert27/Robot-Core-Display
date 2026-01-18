#ifndef MY_FOOTER_H
#define MY_FOOTER_H

#include "lvgl.h"

/**
 * Crea un footer transparente alineado abajo con un icono de acción a la derecha.
 * @param parent Objeto padre/pantalla
 * @param icon_src Icono para el botón de acción (derecha)
 * @param event_cb Callback al pulsar el botón
 * @return Puntero al objeto footer creado
 */
lv_obj_t* create_custom_footer(lv_obj_t* parent, const void* icon_src, lv_event_cb_t event_cb);

#endif // MY_FOOTER_H
