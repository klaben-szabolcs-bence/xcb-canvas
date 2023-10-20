#include "canvas-rendering-context.h"

canvas_rendering_context_t* create_canvas_rendering_context(void (*draw_function) ()) {

    canvas_rendering_context_t* ctx = malloc(sizeof(canvas_rendering_context_t));

    xcbcanvas_t* canvas = xcbcanvas_init_xcb();
    ctx->canvas = canvas;

    printf("xcbcanvas_init_xcb() succeeded\n");

    ctx->font = malloc(sizeof(xcbcanvas_font_t));
    xcbcanvas_font_t font;
    font.family = "Times";
    font.size = 12;
    font.weight = "Medium";
    font.italic = 0;
    ctx_update_font(ctx, font);

    printf("xcbcanvas_font_t initialized\n");

    path_t* path = malloc(sizeof(path_t));
    ctx->path = path;

    printf("path_t initialized\n");

    xcbcanvas_handle_events(ctx, ctx->canvas, draw_function);

    return ctx;
}

void ctx_update_font(canvas_rendering_context_t* rendering_context, xcbcanvas_font_t font)
{
    if (font.family != NULL) {
        rendering_context->font->family = font.family;
    }
    if (font.size != 0) {
        rendering_context->font->size = font.size;
    }
    if (font.weight != NULL) {
        rendering_context->font->weight = font.weight;
    }
    rendering_context->font->italic = font.italic;
    char font_name[255];
    sprintf(
        &font_name,
        "*%s*%s-%s-*--%d-*",
        font.family,
        font.weight,
        font.italic ? "I" : "R",
        font.size
    );
    xcbcanvas_load_font(rendering_context->canvas, font_name);
}