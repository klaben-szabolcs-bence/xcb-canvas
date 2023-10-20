#ifndef XCB_CANVAS_WRAP_H
#define XCB_CANVAS_WRAP_H

#include <stdint.h>

typedef struct __wrap_canvas_rendering_context_t {
    uint32_t foreground;
    uint32_t background;
    void (*draw_function) ();
} __wrap_canvas_rendering_context_t;

int __wrap_xcbcanvas_init_xcb(__wrap_canvas_rendering_context_t* rendering_context);

#endif /* XCB_CANVAS_WRAP_H */