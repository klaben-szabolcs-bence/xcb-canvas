#include "canvas.h"

int canvas_init(canvas_rendering_context_t* rendering_context)
{
    if (!xcbcanvas_init_xcb(rendering_context))
        return -1;

    return 0;
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
    xcb_void_cookie_t       cookie_gc;
    xcb_void_cookie_t       cookie_text;
    xcb_generic_error_t* error;
    xcb_gcontext_t          gc;
    uint8_t                 length;

    length = strlen(text);

    gc = gc_font_get(rendering_context, "7x13");

    cookie_text = xcb_image_text_8_checked(
        rendering_context->c,
        length,
        rendering_context->win,
        rendering_context->foreground,
        x,
        y,
        text
    );

    error = xcb_request_check(rendering_context->c, cookie_text);

    if (error) {
        fprintf(stderr, "Error: Can't paste text: %d\n", error->error_code);
        free(error);
    }
}