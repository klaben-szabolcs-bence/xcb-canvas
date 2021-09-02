#ifndef XCB_CANVAS_H
#define XCB_CANVAS_H

#include <stdlib.h>
#include <stdio.h>

#include <xcb/xcb.h>

/* Prints pressed modifier keys */
void print_modifiers (uint32_t mask);

/* Program entry-point, when executed */
int init_xcb ();

/* Handle the event loop of the main program */
void handle_events (xcb_connection_t *c);
#endif /* XCB_CANVAS_H */
