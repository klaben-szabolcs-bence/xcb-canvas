#include "example.h"

int main ()
{
    canvas_init(&rendering_context);
}

void draw()
{
    canvas_stroke_rectangle(&rendering_context, 0, 0, 100, 100);
}