#ifndef XCB_CANVAS_WRAP_H
#define XCB_CANVAS_WRAP_H

#include <stdint.h>

#ifndef XCBCANVAS_ENCAP_TYPES_IN_H
#define XCBCANVAS_ENCAP_TYPES_IN_H
typedef struct arc_t arc_t; // The rest of the data needed to draw an arc.
typedef enum sub_path_type_t sub_path_type_t;
typedef struct sub_path_t sub_path_t;
#endif /* XCBCANVAS_ENCAP_TYPES_IN_H */

#ifndef XCBCANVAS_SIZE_T
#define XCBCANVAS_SIZE_T
typedef struct xcbcanvas_size_t {
    int width;
    int height;
} xcbcanvas_size_t;
#endif /* XCBCANVAS_SIZE_T */

#ifndef XCBCANVAS_PATH_T
#define XCBCANVAS_PATH_T
typedef struct path_t {
    sub_path_t* sub_paths; // An array of sub paths. Should be allocated in chunks of 10.
    int sub_path_count;
    _Bool filled;
} path_t;
#endif /* XCBCANVAS_PATH_T */

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

void xcbcanvas_draw_path_test(void** state);

#endif /* XCB_CANVAS_WRAP_H */