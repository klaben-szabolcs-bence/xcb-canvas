#ifndef XCB_CANVAS_H
#define XCB_CANVAS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <xcb/xcb.h>

typedef struct arc_t arc_t; // The rest of the data needed to draw an arc.
typedef enum sub_path_type_t sub_path_type_t;
typedef struct sub_path_t sub_path_t;

#ifndef XCBCANVAS_SIZE_T
#define XCBCANVAS_SIZE_T
typedef struct xcbcanvas_size_t {
    int width;
    int height;
} xcbcanvas_size_t;
#endif /* XCBCANVAS_SIZE_T */

typedef struct xcbcanvas_t xcbcanvas_t;

typedef struct path_t {
    sub_path_t* sub_paths; // An array of sub paths. Should be allocated in chunks of 10.
    int sub_path_count;
    _Bool filled;
} path_t;

typedef struct canvas_rendering_context_t {
    xcbcanvas_t* canvas;
    uint32_t fillColor;
    uint32_t strokeColor;
    void (*draw_function) ();
    path_t* path;

} canvas_rendering_context_t;

/* Prints pressed modifier keys */
void xcbcanvas_print_modifiers(uint32_t mask);

/* Program entry-point, when executed */
int xcbcanvas_init_xcb(canvas_rendering_context_t* rendering_context);

/* Set size of the window */
void xcbcanvas_set_window_size(canvas_rendering_context_t* rendering_context, int new_width, int new_height);

/* Get size of window */
xcbcanvas_size_t xcbcanvas_get_window_size(canvas_rendering_context_t* rendering_context);

/* Handle the event loop of the main program */
void xcbcanvas_handle_events(canvas_rendering_context_t* rendering_context);

/* Handle font loading */
xcb_gc_t gc_font_get(canvas_rendering_context_t* rendering_context, char* font_name);

/* Set the color used for drawing */
void xcbcanvas_set_color(canvas_rendering_context_t* rendering_context, uint32_t color);

/* Set the line width used for drawing */
void xcbcanvas_set_stroke_width(canvas_rendering_context_t* rendering_context, int width);

/* Create a rectangle outline */
void xcbcanvas_stroke_rectangle(canvas_rendering_context_t* rendering_context, int16_t x, int16_t y, uint16_t width, uint16_t height);

/* Create a rectangle filled with the fill color */
void xcbcanvas_fill_rectangle(canvas_rendering_context_t* rendering_context, int16_t x, int16_t y, uint16_t width, uint16_t height);

/* Draw text */
void xcbcanvas_draw_text(canvas_rendering_context_t* rendering_context, int16_t x, int16_t y, const char* text);

/* Draw a line */
void xcbcanvas_line(canvas_rendering_context_t* rendering_context, int16_t x1, int16_t y1, int16_t x2, int16_t y2);

/* Draw an arc */
void xcbcanvas_arc(canvas_rendering_context_t* rendering_context, int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t angle1, uint16_t angle2);

/* Draw a filled arc */
void xcbcanvas_fill_arc(canvas_rendering_context_t* rendering_context, int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t angle1, uint16_t angle2);

/* Draw the stored path */
void xcbcanvas_draw_path(canvas_rendering_context_t* rendering_context);

#endif /* XCB_CANVAS_H */
