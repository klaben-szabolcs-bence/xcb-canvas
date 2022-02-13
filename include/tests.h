#ifndef XCB_CANVAS_TESTS_H
#define XCB_CANVAS_TESTS_H

// Inits the tests
int setup(void** state);

// Frees the tests
int teardown(void** state);

// Starts the tests
int main(void);

// Test that always passes
static void null_test_success(void** state);

// Test that always fails
static void null_test_fail(void** state);

#endif // XCB_CANVAS_TESTS_H