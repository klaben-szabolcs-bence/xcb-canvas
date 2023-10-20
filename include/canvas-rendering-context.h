#ifndef CANVAS_RENDERING_CONTEXT_H
#define CANVAS_RENDERING_CONTEXT_H

#include "xcb-canvas.h"

typedef struct xcbcanvas_font_t {
    char* family;
    uint32_t size;
    char* weight;
    _Bool italic;
} xcbcanvas_font_t;

struct canvas_rendering_context_t {
    xcbcanvas_t* canvas;
    uint32_t fillColor;
    uint32_t strokeColor;
    path_t* path;
    xcbcanvas_font_t* font;
};

/** @brief Creates a canvas and initializes it.
 *
 * @returns A canvas.
 */
canvas_rendering_context_t* create_canvas_rendering_context(void (*draw_function) ());

/**
 * @brief Updates the font of the canvas.
 *
 * @param rendering_context The rendering context to use.
 * @param font              The font to use.
 */
void ctx_update_font(canvas_rendering_context_t* rendering_context, xcbcanvas_font_t font);

#endif /* CANVAS_RENDERING_CONTEXT_H */