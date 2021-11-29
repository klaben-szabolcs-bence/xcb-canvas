#include "canvas.h"

void canvas_init (canvas_rendering_context_t* rendering_context)
{
    xcbcanvas_init_xcb(rendering_context);
}

void canvas_stroke_rectangle(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    uint16_t width, uint16_t height
    )
{
    xcb_rectangle_t rectangle = {
        .x = x,
        .y = y,
        .width = width,
        .height = height
    };
    xcb_poly_rectangle(rendering_context->c, rendering_context->win, rendering_context->foreground, 1, &rectangle);
}

void canvas_fill_rectangle(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    uint16_t width, uint16_t height
    )
{
    xcb_rectangle_t rectangle = {
        .x = x,
        .y = y,
        .width = width,
        .height = height
    };
    xcb_poly_fill_rectangle(rendering_context->c, rendering_context->win, rendering_context->foreground, 1, &rectangle);
}

void canvas_draw_text(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    const char* text
    )
{
    xcb_image_text_16(rendering_context->c, strlen(text), rendering_context->win, rendering_context->foreground,
        x, y, text);
}