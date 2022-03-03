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

int __wrap_xcbcanvas_init_xcb(__wrap_canvas_rendering_context_t* rendering_context)
{
    rendering_context->window_size.width = 800;
    rendering_context->window_size.height = 600;
    rendering_context->background = 0;
    rendering_context->foreground = 1;
    __wrap_xcbcanvas_handle_events(rendering_context);
    return 0;
}

void __wrap_xcbcanvas_set_window_size(__wrap_canvas_rendering_context_t* rendering_context, int new_width, int new_height)
{
    rendering_context->window_size.width = new_width;
    rendering_context->window_size.height = new_height;
}

xcbcanvas_size_t __wrap_xcbcanvas_get_window_size(__wrap_canvas_rendering_context_t* rendering_context) {
    return rendering_context->window_size;
}

void __wrap_xcbcanvas_handle_events(__wrap_canvas_rendering_context_t* rendering_context)
{
    rendering_context->draw_function();
}

