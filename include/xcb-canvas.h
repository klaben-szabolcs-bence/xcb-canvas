#ifndef XCB_CANVAS_H
#define XCB_CANVAS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <xcb/xcb.h>

typedef struct canvas_size_t {
    int width;
    int height;
} canvas_size_t;

typedef struct canvas_rendering_context_t {
    xcb_connection_t* c;
    xcb_window_t win;
    xcb_screen_t* screen;
    xcb_gcontext_t foreground;
    xcb_gcontext_t background;
    void (*draw_function) ();
} canvas_rendering_context_t;

/* TODO: Add color and line stlye support */

/* Prints pressed modifier keys */
void xcbcanvas_print_modifiers(uint32_t mask);

/* Program entry-point, when executed */
int xcbcanvas_init_xcb(canvas_rendering_context_t* rendering_context);

/* Set size of the window */
void xcbcanvas_set_window_size(canvas_rendering_context_t* rendering_context, int new_width, int new_height);

/* Get size of window */
canvas_size_t xcbcanvas_get_windows_size(canvas_rendering_context_t* rendering_context);

/* Set the draw function */
void xcbcanvas_set_draw_function(canvas_rendering_context_t* rendering_context, void (*draw_function) ());

/* Handle the event loop of the main program */
void xcbcanvas_handle_events(canvas_rendering_context_t* rendering_context);

/* Handle font loading */
xcb_gc_t gc_font_get(canvas_rendering_context_t* rendering_context, char* font_name);

#endif /* XCB_CANVAS_H */
