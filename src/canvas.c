#include "canvas.h"

// TODO: Refactor xcb calls from this file to use xcb-canvas.h methods.

int canvas_init(canvas_rendering_context_t* rendering_context)
{
    int rc = xcbcanvas_init_xcb(rendering_context);
    if (rc != 0) {
        return -1;
    }

    return 0;
}

void canvas_stroke_rectangle(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    uint16_t width, uint16_t height
)
{
    xcbcanvas_stroke_rectangle(rendering_context, x, y, width, height);
}

void canvas_fill_rectangle(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    uint16_t width, uint16_t height
)
{
    xcbcanvas_fill_rectangle(rendering_context, x, y, width, height);
}

void canvas_draw_text(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    const char* text
)
{
   xcbcanvas_draw_text(rendering_context, x, y, text);
}