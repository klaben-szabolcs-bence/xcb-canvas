#include <canvas-tests.h>

// TODO: Add tests for canvas.c

void canvas_init_test(void** state)
{
    int rc = canvas_init(((struct state*)state)->rendering_context);
    assert_return_code(rc, 0);
}