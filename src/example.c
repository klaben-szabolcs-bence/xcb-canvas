#include "example.h"

int main()
{
    xcbcanvas_set_draw_function(&rendering_context, draw);
    
    if (!canvas_init(&rendering_context))
    {
        printf("Error initialising canvas\n");
        return 1;
    }
}

void draw()
{
    canvas_stroke_rectangle(&rendering_context, 0, 0, 100, 100);
}