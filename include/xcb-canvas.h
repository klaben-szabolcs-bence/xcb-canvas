#ifndef XCB_CANVAS_H
#define XCB_CANVAS_H

#include <stdlib.h>
#include <stdio.h>

#include <xcb/xcb.h>

typedef struct RenderingContext {
    int width;
    int height;
    xcb_window_t win;
    xcb_connection_t *c;
} RenderingContext;

RenderingContext rendering_context;

/* Prints pressed modifier keys */
void print_modifiers (uint32_t mask);

/* Program entry-point, when executed */
int init_xcb ();

/* Get/Set the RenderingContext's properties */
void set_window_size (RenderingContext* rendering_context, int new_width, int new_height);

/* Handle the event loop of the main program */
void handle_events (xcb_connection_t *c);
#endif /* XCB_CANVAS_H */
