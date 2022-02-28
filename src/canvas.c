#include "canvas.h"

/* Encapsulated types */
struct position_t {
    int16_t x;
    int16_t y;
};

struct angle_t {
    uint16_t degree;
};

enum sub_path_type_t {
    SUBPATH_TYPE_MOVE,
    SUBPATH_TYPE_LINE,
    SUBPATH_TYPE_ARC,
    SUBPATH_TYPE_QUADRATIC_CURVE,
    SUBPATH_TYPE_CUBIC_CURVE,
    SUBPATH_TYPE_CLOSE
};

struct sub_path_t {
    position_t* points;
    sub_path_type_t type;
};

int canvas_init(canvas_rendering_context_t* rendering_context)
{
    int rc = xcbcanvas_init_xcb(rendering_context);
    if (rc != 0) {
        return -1;
    }

    return 0;
}

void canvas_dealloc(canvas_rendering_context_t* rendering_context)
{
    free(rendering_context->canvas);
    free(rendering_context->path);
}

void canvas_set_draw_function(canvas_rendering_context_t* rendering_context, void (*draw_function)())
{
    rendering_context->draw_function = draw_function;
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

void canvas_begin_path(
    canvas_rendering_context_t* rendering_context
)
{
    rendering_context->path->sub_path_count = 0;
    rendering_context->path->sub_paths = malloc(sizeof(sub_path_t) * 10);
}

void canvas_move_to(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y
)
{
    if (rendering_context->path->sub_path_count % 10 == 9)
    {
        rendering_context->path->sub_paths = realloc(rendering_context->path->sub_paths, sizeof(sub_path_t) * (rendering_context->path->sub_path_count + 10));
    }
    sub_path_t* sub_path = &rendering_context->path->sub_paths[rendering_context->path->sub_path_count];
    sub_path->points = malloc(sizeof(position_t) * 2);
    sub_path->points[0].x = x;
    sub_path->points[0].y = y;
    sub_path->type = SUBPATH_TYPE_MOVE;
    rendering_context->path->sub_path_count++;
}

void canvas_line_to(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y
)
{
    if (rendering_context->path->sub_path_count % 10 == 9)
    {
        rendering_context->path->sub_paths = realloc(rendering_context->path->sub_paths, sizeof(sub_path_t) * (rendering_context->path->sub_path_count + 10));
    }
    sub_path_t* sub_path = &rendering_context->path->sub_paths[rendering_context->path->sub_path_count];
    sub_path->points = malloc(sizeof(position_t) * 2);
    sub_path->points[0].x = rendering_context->path->sub_paths[rendering_context->path->sub_path_count - 1].points[0].x;
    sub_path->points[0].y = rendering_context->path->sub_paths[rendering_context->path->sub_path_count - 1].points[0].y;
    sub_path->points[1].x = x;
    sub_path->points[1].y = y;
    sub_path->type = SUBPATH_TYPE_LINE;
    rendering_context->path->sub_path_count++;
}

void canvas_arc(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    uint16_t radius,
    angle_t start_angle,
    angle_t end_angle
)
{
    if (rendering_context->path->sub_path_count % 10 == 9)
    {
        rendering_context->path->sub_paths = realloc(rendering_context->path->sub_paths, sizeof(sub_path_t) * (rendering_context->path->sub_path_count + 10));
    }
    sub_path_t* sub_path = &rendering_context->path->sub_paths[rendering_context->path->sub_path_count];
    sub_path->points = malloc(sizeof(position_t) * 2);
    sub_path->points[0].x = x;
    sub_path->points[0].y = y;
    sub_path->points[1].x = radius;
    sub_path->points[1].y = start_angle.degree;
    sub_path->points[2].x = end_angle.degree;
    sub_path->type = SUBPATH_TYPE_ARC;
    rendering_context->path->sub_path_count++;
}

void canvas_close_path(
    canvas_rendering_context_t* rendering_context
)
{
    if (rendering_context->path->sub_path_count % 10 == 9)
    {
        rendering_context->path->sub_paths = realloc(rendering_context->path->sub_paths, sizeof(sub_path_t) * (rendering_context->path->sub_path_count + 10));
    }
    sub_path_t* sub_path = &rendering_context->path->sub_paths[rendering_context->path->sub_path_count];
    sub_path->points = malloc(sizeof(position_t) * 2);
    sub_path->points[0].x = rendering_context->path->sub_paths[0].points[0].x;
    sub_path->points[0].y = rendering_context->path->sub_paths[0].points[0].y;
    sub_path->points[1].x = rendering_context->path->sub_paths[rendering_context->path->sub_path_count - 1].points[0].x;
    sub_path->points[1].y = rendering_context->path->sub_paths[rendering_context->path->sub_path_count - 1].points[0].y;
    sub_path->type = SUBPATH_TYPE_CLOSE;
    rendering_context->path->sub_path_count++;
}

void canvas_stroke(
    canvas_rendering_context_t* rendering_context
)
{
    xcbcanvas_draw_path(rendering_context);
    for (int i = 0; i < rendering_context->path->sub_path_count; i++)
    {
        free(rendering_context->path->sub_paths[i].points);
    }
}

void canvas_fill(
    canvas_rendering_context_t* rendering_context
)
{
    rendering_context->path->filled = 1;
    canvas_close_path(rendering_context);
    xcbcanvas_draw_path(rendering_context);
    for (int i = 0; i < rendering_context->path->sub_path_count; i++)
    {
        free(rendering_context->path->sub_paths[i].points);
    }
}