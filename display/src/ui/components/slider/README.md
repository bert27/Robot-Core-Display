# MySlider Component 🎚️

A custom slider for touch interfaces in LVGL, optimized for small screens (4.3") where ease of interaction is required.

## Features ✨

-   **Knob XL**: Oversized grip "ball" (10px padding) to facilitate finger dragging.
-   **Extended Touch Area**: The touch detection area is 20px larger than the visual slider, improving usability.
-   **"Chunky" Style**: Thick bar (20px) with high-contrast colors (Dark Gray Background + Neon Green Indicator).
-   **Flexible Width**: Can be configured with fixed width (px) or percentage (`LV_PCT(x)`).

## Usage 🛠️

```cpp
#include "../components/slider/MySlider.h"

// Event callback (optional)
static void my_slider_cb(lv_event_t * e) {
    lv_obj_t * slider = lv_event_get_target(e);
    int value = lv_slider_get_value(slider);
    // ... do something with the value
}

// Create slider in a container
void create_my_ui(lv_obj_t * parent) {
    // create_custom_slider(parent, min, max, width, callback)
    lv_obj_t * slider = create_custom_slider(parent, 0, 100, LV_PCT(90), my_slider_cb);
    
    // Optional: Set initial value
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);
}
```

## Files

-   `MySlider.h`: Declaration.
-   `MySlider.cpp`: Implementation and styles.
