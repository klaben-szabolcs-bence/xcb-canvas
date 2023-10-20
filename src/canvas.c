#include "canvas.h"

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

void canvas_clear_rectangle(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    uint16_t width, uint16_t height
)
{
    uint32_t stack_color = rendering_context->strokeColor;
    xcbcanvas_set_color(rendering_context, 0);
    xcbcanvas_fill_rectangle(rendering_context, x, y, width, height);
    xcbcanvas_set_color(rendering_context, stack_color);
}

void canvas_draw_text(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    const char* text
)
{
    xcbcanvas_draw_text(rendering_context, x, y, text);
}

void canvas_set_color(
    canvas_rendering_context_t* rendering_context,
    uint8_t red, uint8_t green, uint8_t blue
)
{
    uint32_t color = (red << 16) | (green << 8) | blue;
    xcbcanvas_set_color(rendering_context, color);
}

void canvas_set_line_width(
    canvas_rendering_context_t* rendering_context,
    uint16_t width
)
{
    xcbcanvas_set_stroke_width(rendering_context, width);
}