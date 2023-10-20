#include "xcb-canvas.h"

void xcbcanvas_print_modifiers(uint32_t mask)
{
  const char** mod, * mods[] = {
                        "Shift", "Lock", "Ctrl", "Alt",
                        "NumLock", "AltGr", "Win", "ScrollLock",
                        "Button1", "Button2", "Button3", "Button4", "Button5" };
  printf("Modifier mask: ");
  for (mod = mods; mask; mask >>= 1, mod++)
    if (mask & 1)
      printf("%s+", *mod);
  putchar('\n');
}

int xcbcanvas_init_xcb(canvas_rendering_context_t* rendering_context)
{
  xcb_connection_t* c;
  xcb_screen_t* screen;
  xcb_window_t win;
  xcb_gcontext_t foreground;
  xcb_gcontext_t background;
  uint32_t mask = 0;
  uint32_t values[2];

  /* Open the connection to the X server */
  c = xcb_connect(NULL, NULL);

  if (xcb_connection_has_error(c))
    {
      printf("Error: Can't open display: %s\n", xcb_get_error_text(c, xcb_connection_has_error(c)));
      return -1;
    }

  /* Get the first screen */
  screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;

  if (screen == NULL)
    {
      printf("Error: Can't get screen\n");
      return -1;
    }

  /* Create black (foreground) and white (background) colors */
  foreground = xcb_generate_id(c);
  mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = screen->black_pixel;
  values[1] = 0;
  xcb_create_gc(c, foreground, screen->root, mask, values);
  background = xcb_generate_id(c);
  mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = screen->white_pixel;
  values[1] = 0;
  xcb_create_gc(c, background, screen->root, mask, values);

  /* Ask for our window's Id */
  win = xcb_generate_id(c);

  if (win == 0)
    {
      printf("Error: Can't get window id\n");
      return -1;
    }

  /* Create the window */
  mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
  values[0] = screen->white_pixel;
  values[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
    XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
    XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
    XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE;
  xcb_create_window(c,             /* Connection          */
    0,                             /* depth               */
    win,                           /* window Id           */
    screen->root,                  /* parent window       */
    10, 10,                        /* x, y                */
    800, 600,                      /* width, height       */
    10,                            /* border_width        */
    XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
    screen->root_visual,           /* visual              */
    mask, values);                 /* masks */

  if (xcb_connection_has_error(c))
    {
      printf("Error: Can't create window: %s\n", xcb_get_error_text(c, xcb_connection_has_error(c)));
      return -1;
    }
  
  /* Map the window on the screen */
  xcb_map_window(c, win);

  if (xcb_connection_has_error(c))
    {
      printf("Error: Can't map window: %s\n", xcb_get_error_text(c, xcb_connection_has_error(c)));
      return -1;
    }

  xcb_flush(c);

  rendering_context->win = win;
  rendering_context->c = c;
  rendering_context->screen = screen;
  rendering_context->foreground = foreground;
  rendering_context->background = background;

  xcbcanvas_handle_events(rendering_context);

  return 0;
}

void xcbcanvas_handle_events(canvas_rendering_context_t* rendering_context)
{
  xcb_connection_t* c = rendering_context->c;
  xcb_generic_event_t* e;
  while ((e = xcb_wait_for_event(c)))
  {
    switch (e->response_type & ~0x80)
    {
      case XCB_EXPOSE:
      {
        xcb_expose_event_t* ev = (xcb_expose_event_t*)e;
        rendering_context->draw_function();
        /* printf ("Window %u exposed. Region to be redrawn at location (%d,%d), with dimension (%d,%d)\n",
                ev->window, ev->x, ev->y, ev->width, ev->height); */
        xcb_flush(c);
        break;
      }
      case XCB_BUTTON_PRESS:
      {
        xcb_button_press_event_t* ev = (xcb_button_press_event_t*)e;
        /* print_modifiers(ev->state); */

        switch (ev->detail)
        {
          case 4:
            /* printf ("Wheel Button up in window %u, at coordinates (%d,%d)\n",
                    ev->event, ev->event_x, ev->event_y); */
            break;
          case 5:
            /* printf ("Wheel Button down in window %u, at coordinates (%d,%d)\n",
                    ev->event, ev->event_x, ev->event_y); */
            break;
          default:
            /* printf ("Button %d pressed in window %u, at coordinates (%d,%d)\n",
                    ev->detail, ev->event, ev->event_x, ev->event_y); */
            break;
        }
        break;
      }
      case XCB_BUTTON_RELEASE:
      {
        xcb_button_release_event_t* ev = (xcb_button_release_event_t*)e;
        /* print_modifiers(ev->state); */

        /* printf ("Button %d released in window %u, at coordinates (%d,%d)\n",
                ev->detail, ev->event, ev->event_x, ev->event_y); */
        break;
      }
      case XCB_MOTION_NOTIFY:
      {
        xcb_motion_notify_event_t* ev = (xcb_motion_notify_event_t*)e;

        /* printf ("Mouse moved in window %u, at coordinates (%d,%d)\n",
                ev->event, ev->event_x, ev->event_y); */
        break;
      }
      case XCB_ENTER_NOTIFY:
      {
        xcb_enter_notify_event_t* ev = (xcb_enter_notify_event_t*)e;

        /* printf ("Mouse entered window %u, at coordinates (%d,%d)\n",
                ev->event, ev->event_x, ev->event_y); */
        break;
      }
      case XCB_LEAVE_NOTIFY:
      {
        xcb_leave_notify_event_t* ev = (xcb_leave_notify_event_t*)e;

        /* printf ("Mouse left window %u, at coordinates (%d,%d)\n",
                ev->event, ev->event_x, ev->event_y); */
        break;
      }
      case XCB_KEY_PRESS:
      {
        xcb_key_press_event_t* ev = (xcb_key_press_event_t*)e;
        /* print_modifiers(ev->state); */

        /* printf ("Key pressed in window %u\n",
                ev->event); */
        break;
      }
      case XCB_KEY_RELEASE:
      {
        xcb_key_release_event_t* ev = (xcb_key_release_event_t*)e;
        /* print_modifiers(ev->state); */

        /* printf ("Key released in window %u\n",
                ev->event); */
        break;
      }
      default:
        /* Unknown event type, ignore it */
        printf("Unknown event: %d\n", e->response_type);
        break;
    }
    /* Free the Generic Event */
    free(e);
  }
}

void xcbcanvas_set_window_size(canvas_rendering_context_t* rendering_context, int new_width, int new_height)
{
  static uint32_t values[2];
  values[0] = new_width;
  values[1] = new_height;
  xcb_configure_window(rendering_context->c, rendering_context->win,
    XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
}

canvas_size_t xcbcanvas_get_windows_size(canvas_rendering_context_t* rendering_context)
{
  xcb_get_geometry_cookie_t cookie = xcb_get_geometry(rendering_context->c, rendering_context->win);
  xcb_get_geometry_reply_t* reply = xcb_get_geometry_reply(rendering_context->c, cookie, NULL);
  canvas_size_t size;
  size.width = reply->width;
  size.height = reply->height;
  free(reply);
  return size;
}

void xcbcanvas_set_draw_function(canvas_rendering_context_t* rendering_context, void (*draw_function)())
{
  rendering_context->draw_function = draw_function;
}

xcb_gc_t gc_font_get(canvas_rendering_context_t* rendering_context, char* font_name)
{

  uint32_t value_list[3];
  xcb_void_cookie_t cookie_font;
  xcb_void_cookie_t cookie_gc;
  xcb_generic_error_t* error;
  xcb_font_t font;
  xcb_gcontext_t gc;
  uint32_t mask;

  xcb_connection_t* c = rendering_context->c;
  xcb_screen_t* screen = rendering_context->screen;
  xcb_window_t win = rendering_context->win;

  font = xcb_generate_id(c);
  cookie_font = xcb_open_font_checked(c, font,
    strlen(font_name),
    font_name);

  error = xcb_request_check(c, cookie_font);
  if (error)
  {
    fprintf(stderr, "ERROR: can't open font : %d\n", error->error_code);
    xcb_disconnect(c);
    exit(-1);
  }

  gc = xcb_generate_id(c);
  mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_FONT;
  value_list[0] = screen->black_pixel;
  value_list[1] = screen->white_pixel;
  value_list[2] = font;
  cookie_gc = xcb_create_gc_checked(c, gc, win, mask, value_list);
  error = xcb_request_check(c, cookie_gc);
  if (error)
  {
    fprintf(stderr, "ERROR: can't create gc : %d\n", error->error_code);
    xcb_disconnect(c);
    exit(-1);
  }

  cookie_font = xcb_close_font_checked(c, font);
  error = xcb_request_check(c, cookie_font);
  if (error)
  {
    fprintf(stderr, "ERROR: can't close font : %d\n", error->error_code);
    xcb_disconnect(c);
    exit(-1);
  }

  return (xcb_gc_t)gc;
}