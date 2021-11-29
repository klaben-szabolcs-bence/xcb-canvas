#include "canvas.h"

void canvas_init (canvas_rendering_context_t* rendering_context)
{
    xcbcanvas_init_xcb(rendering_context);
}

void canvas_draw_point (canvas_rendering_context_t* rendering_context, int16_t x, int16_t y)
{
    xcb_point_t point = {x, y};
    xcb_poly_point(rendering_context->c, XCB_COORD_MODE_ORIGIN, rendering_context->win, rendering_context->foreground,
        1, &point);
}

void canvas_draw_line (canvas_rendering_context_t* rendering_context, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    xcb_segment_t segment = {
        .x1 = x1,
        .y1 = y1,
        .x2 = x2,
        .y2 = y2
    };
    xcb_poly_segment(rendering_context->c, rendering_context->win, rendering_context->foreground, 1, &segment);
}

void canvas_draw_rectangle(
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

void canvas_draw_arc(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    uint16_t width, uint16_t height,
    int16_t angle1, int16_t angle2
    )
{
    xcb_arc_t arc = {
        .x = x,
        .y = y,
        .width = width,
        .height = height,
        .angle1 = angle1,
        .angle2 = angle2
    };
    xcb_poly_arc(rendering_context->c, rendering_context->win, rendering_context->foreground, 1, &arc);
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

void canvas_fill_arc(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    uint16_t width, uint16_t height,
    int16_t angle1, int16_t angle2
    )
{
    xcb_arc_t arc = {
        .x = x,
        .y = y,
        .width = width,
        .height = height,
        .angle1 = angle1,
        .angle2 = angle2
    };
    xcb_poly_fill_arc(rendering_context->c, rendering_context->win, rendering_context->foreground, 1, &arc);
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