#include "example.h"
#include <time.h>

int main()
{
    canvas_rendering_context_t* rendering_context = create_canvas_rendering_context(draw);
    canvas_dealloc(rendering_context);

    return 0;
}

#define MEASURE_DRAW_TIME // comment this line to disable time measurement
void draw(canvas_rendering_context_t* rendering_context)
{
#ifdef MEASURE_DRAW_TIME
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
#endif
    canvas_fill_rectangle(rendering_context, 0, 0, 100, 100);
    canvas_set_line_width(rendering_context, 20);
    canvas_set_color(rendering_context, 255, 0, 0);
    canvas_stroke_rectangle(rendering_context, 50, 50, 100, 100);

    /* Draw a blue triangle */
    canvas_set_color(rendering_context, 0, 0, 255);
    canvas_set_line_width(rendering_context, 5);
    canvas_begin_path(rendering_context);
    canvas_move_to(rendering_context, 200, 200);
    canvas_line_to(rendering_context, 300, 200);
    canvas_line_to(rendering_context, 300, 300);
    canvas_close_path(rendering_context);
    canvas_fill(rendering_context);

    /* Draw a green circle */
    canvas_set_color(rendering_context, 0, 255, 0);
    canvas_set_line_width(rendering_context, 10);
    canvas_begin_path(rendering_context);
    canvas_arc(rendering_context, 300, 500, 100, 0, 360, 0);
    canvas_close_path(rendering_context);
    canvas_fill(rendering_context);

    /* Write Hello World */
    canvas_set_color(rendering_context, 0, 0, 0);
    canvas_set_line_width(rendering_context, 1);
    canvas_font2(rendering_context, "helvetica", 24);
    canvas_fill_text(rendering_context, "Hello World", 500, 200);

#ifdef MEASURE_DRAW_TIME
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("Time taken: %f\n", (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0);
#endif
}