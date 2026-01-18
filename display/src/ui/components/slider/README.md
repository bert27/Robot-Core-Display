# Componente MySlider 🎚️

Un slider personalizado para interfaces táctiles en LVGL, optimizado para pantallas pequeñas (4.3") donde se requiere facilidad de interacción.

## Características ✨

-   **Knob XL**: "Bola" de agarre sobredimensionada (padding 10px) para facilitar el arrastre con el dedo.
-   **Área Táctil Extendida**: El área de detección de toques es 20px más grande que el slider visual, mejorando la usabilidad.
-   **Estilo "Chunky"**: Barra gruesa (20px) con colores de alto contraste (Fondo Gris Oscuro + Indicador Verde Neón).
-   **Ancho Flexible**: Puede configurarse con ancho fijo (px) o porcentaje (`LV_PCT(x)`).

## Uso 🛠️

```cpp
#include "../components/slider/MySlider.h"

// Callback de evento (opcional)
static void mi_slider_cb(lv_event_t * e) {
    lv_obj_t * slider = lv_event_get_target(e);
    int valor = lv_slider_get_value(slider);
    // ... hacer algo con el valor
}

// Crear slider en un contenedor
void crear_mi_ui(lv_obj_t * parent) {
    // create_custom_slider(padre, min, max, ancho, callback)
    lv_obj_t * slider = create_custom_slider(parent, 0, 100, LV_PCT(90), mi_slider_cb);
    
    // Opcional: Establecer valor inicial
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);
}
```

## Archivos

-   `MySlider.h`: Declaración.
-   `MySlider.cpp`: Implementación y estilos.
