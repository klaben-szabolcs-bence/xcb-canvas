#ifndef XCB_CANVAS_TESTS_H
#define XCB_CANVAS_TESTS_H

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <xcb-canvas-wrap.h>
#include <canvas-tests.h>

struct state {
    __wrap_canvas_rendering_context_t* rendering_context;
};

// Inits the state for the tests
int setup(void** state);

// Frees the state for the tests
int teardown(void** state);

// Starts the tests
int main(void);

// Test that always passes
static void null_test_success(void** state);

// Test that always fails
static void null_test_fail(void** state);

#endif // XCB_CANVAS_TESTS_H