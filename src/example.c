#include "example.h"

int main()
{
    canvas_set_draw_function(&rendering_context, draw);

    if (!canvas_init(&rendering_context))
    {
        printf("Error initialising canvas\n");
        return 1;
    }

    canvas_dealloc(&rendering_context);
}

void draw()
{
    canvas_fill_rectangle(&rendering_context, 0, 0, 100, 100);
    canvas_set_line_width(&rendering_context, 20);
    canvas_set_color(&rendering_context, 255, 0, 0);
    canvas_stroke_rectangle(&rendering_context, 50, 50, 100, 100);
}