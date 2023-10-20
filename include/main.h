#ifndef MAIN_H
#define MAIN_H

#include <stdlib.h>
#include <stdio.h>

#include <xcb/xcb.h>

/* Prints pressed modifier keys */
void print_modifiers (uint32_t mask);

/* Program entry-point, when executed */
int main ();

/* Handle the event loop of the main program */
void handle_events (xcb_connection_t *c);
#endif /* MAIN_H */
