#include <xcb-canvas-wrap.h>

/*
 * This is a mocked object, as such it does not create a window.
 *
 * This object is also wrapped - if any code links with this file and is
 * compiled with linker option --wrap xcbcanvas_init_xcb, any calls to the real function
 * will be redirected to this file.
 *
 * If for any reason the wrapped function wanted to call the real xcbcanvas_init_xcb()
 * functions, it could do so by calling the special symbol __real_xcbcanvas_init_xcb().
 */

 // TODO: This is a stub, it should be implemented.
int __wrap_xcbcanvas_init_xcb(__wrap_canvas_rendering_context_t* rendering_context)
{
    return 0;
}

// TODO: Create tests for the setters and getters.