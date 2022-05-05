#include <tests.h>
#include <canvas-tests.h>
#include <xcb-canvas-wrap.h>

/* A test case that does nothing and succeeds. */
static void null_test_success(void** state) {
    (void)state; /* unused */
}


static void null_test_fail(void** state)
{
    assert_true(0); // fails because given boolean expression evaluates to 0
}


/* These functions will be used to initialize
   and clean resources up after each test run */
int setup(void** state)
{
    *state = malloc(sizeof(struct state));
    if (*state == NULL) {
        return -1;
    }

    return 0;
}

int teardown(void** state)
{
    free(*state);
    return 0;
}


int main(void)
{
    /*
    const struct CMUnitTest basic_tests[] =
    {
        cmocka_unit_test(null_test_success),
        //cmocka_unit_test(null_test_fail),
    };
    */

    const struct CMUnitTest xcbcanvas_tests[] =
    {
        // TODO: Add tests for xcb-canvas.c
        cmocka_unit_test(xcbcanvas_draw_path_test)
    };

    const struct CMUnitTest canvas_tests[] =
    {
        // TODO: Add tests for canvas.c
        //cmocka_unit_test(canvas_init_test)
    };

    // If setup and teardown functions are not needed, then NULL may be passed instead

    //int count_fail_tests = cmocka_run_group_tests(basic_tests, setup, teardown);
    int count_fail_tests = cmocka_run_group_tests_name("XCB-Canvas Tests", xcbcanvas_tests, setup, teardown);
    count_fail_tests += cmocka_run_group_tests_name("Canvas Tests", canvas_tests, setup, teardown);

    return count_fail_tests;
}