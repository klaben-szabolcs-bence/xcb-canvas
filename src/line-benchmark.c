#include "example.h"
#include <time.h>

int main()
{
    canvas_rendering_context_t* rendering_context = create_canvas_rendering_context(draw);
    canvas_dealloc(rendering_context);

    return 0;
}
void draw(canvas_rendering_context_t* rendering_context)
{
    struct timespec start, end;
    xcbcanvas_set_window_size(rendering_context->canvas, 1280, 720);
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Draw 1280 lines with different colors creating a gradient effect
    canvas_set_line_width(rendering_context, 2);

    for (int i = 1; i < 1280; i++) {
        canvas_begin_path(rendering_context);
        canvas_move_to(rendering_context, (i - 1), (i - 1) % 2 ? 0 : 720);
        canvas_set_color(rendering_context, (int)(i / 1280.0 * 255), (int)(i / 1280.0 * 255), (int)(i / 1280.0 * 255));
        canvas_line_to(rendering_context, i, i % 2 ? 0 : 720);
        canvas_stroke(rendering_context);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("Time taken: %f\n", (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0);
}