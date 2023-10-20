#include <canvas-tests.h>

// TODO: Add tests for canvas.c

void canvas_init_test(void** state)
{
    ((struct state*)state)->rendering_context->draw_function = canvas_draw;
    int rc = canvas_init(((struct state*)state)->rendering_context);
    assert_return_code(rc, 0);
}

void canvas_draw(void** state)
{
}