#ifndef XCB_CANVAS_WRAP_H
#define XCB_CANVAS_WRAP_H

#include <stdint.h>

typedef struct xcbcanvas_size_t {
    int width;
    int height;
} xcbcanvas_size_t;

typedef struct __wrap_canvas_rendering_context_t {
    xcbcanvas_size_t window_size;
    uint32_t foreground;
    uint32_t background;
    void (*draw_function) ();
} __wrap_canvas_rendering_context_t;

int __wrap_xcbcanvas_init_xcb(__wrap_canvas_rendering_context_t* rendering_context);

void __wrap_xcbcanvas_handle_events(__wrap_canvas_rendering_context_t* rendering_context);

void __wrap_xcbcanvas_set_window_size(__wrap_canvas_rendering_context_t* rendering_context, int new_width, int new_height);

xcbcanvas_size_t __wrap_xcbcanvas_get_window_size(__wrap_canvas_rendering_context_t* rendering_context);

#endif /* XCB_CANVAS_WRAP_H */