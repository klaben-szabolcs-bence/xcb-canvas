#include "xcb-canvas.h"

/* Private encapsulation for xcb variables */
struct xcbcanvas_t {
  xcb_connection_t* connection;
  xcb_screen_t* screen;
  xcb_window_t window;
  xcb_gcontext_t gc;
};

#ifndef CANVAS_PATH_TYPES
#define CANVAS_PATH_TYPES
/* Encapsulated types */
struct arc_t {
  uint16_t start_radius_or_cp2_x;
  uint16_t end_radius_or_cp2_y;
  uint16_t radius;
};

enum sub_path_type_t {
  SUBPATH_TYPE_MOVE,
  SUBPATH_TYPE_LINE,
  SUBPATH_TYPE_ARC,
  SUBPATH_TYPE_ARC_TO,
  SUBPATH_TYPE_QUADRATIC_CURVE,
  SUBPATH_TYPE_CUBIC_CURVE,
  SUBPATH_TYPE_CLOSE
};

struct sub_path_t {
  xcb_point_t point;
  arc_t arc;
  sub_path_type_t type;
};
#endif /* CANVAS_PATH_TYPES */

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
  xcb_gcontext_t gc;
  uint32_t mask = 0;
  uint32_t values[2];

  /* Open the connection to the X server */
  c = xcb_connect(NULL, NULL);

  if (xcb_connection_has_error(c))
  {
    printf("Error: Can't open display: %s\n", getenv("DISPLAY"));
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
  gc = xcb_generate_id(c);
  mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = screen->black_pixel;
  values[1] = 0;
  xcb_create_gc(c, gc, screen->root, mask, values);

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
    printf("Error: Can't create window:\n");
    return -1;
  }

  /* Map the window on the screen */
  xcb_map_window(c, win);

  if (xcb_connection_has_error(c))
  {
    printf("Error: Can't map window:\n");
    return -1;
  }


  xcb_flush(c);

  xcbcanvas_t* xcbcanvas = malloc(sizeof(xcbcanvas_t));

  xcbcanvas->connection = c;
  xcbcanvas->screen = screen;
  xcbcanvas->window = win;
  xcbcanvas->gc = gc;
  rendering_context->canvas = xcbcanvas;

  path_t* path = malloc(sizeof(path_t));
  rendering_context->path = path;

  xcbcanvas_handle_events(rendering_context);

  return 0;
}

void xcbcanvas_handle_events(canvas_rendering_context_t* rendering_context)
{
  xcb_connection_t* c = rendering_context->canvas->connection;
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
  xcb_configure_window(rendering_context->canvas->connection, rendering_context->canvas->window,
    XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
}

xcbcanvas_size_t xcbcanvas_get_window_size(canvas_rendering_context_t* rendering_context)
{
  xcb_get_geometry_cookie_t cookie = xcb_get_geometry(rendering_context->canvas->connection, rendering_context->canvas->window);
  xcb_get_geometry_reply_t* reply = xcb_get_geometry_reply(rendering_context->canvas->connection, cookie, NULL);
  xcbcanvas_size_t size;
  size.width = reply->width;
  size.height = reply->height;
  free(reply);
  return size;
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

  xcb_connection_t* c = rendering_context->canvas->connection;
  xcb_screen_t* screen = rendering_context->canvas->screen;
  xcb_window_t win = rendering_context->canvas->window;

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

void xcbcanvas_set_window_title(canvas_rendering_context_t* rendering_context, char* title)
{
  xcb_connection_t* c = rendering_context->canvas->connection;
  xcb_window_t win = rendering_context->canvas->window;
  xcb_change_property(c, XCB_PROP_MODE_REPLACE, win, XCB_ATOM_WM_NAME,
    XCB_ATOM_STRING, 8, strlen(title), title);
}

void xcbcanvas_set_window_position(canvas_rendering_context_t* rendering_context, int x, int y)
{
  static uint32_t values[2];
  values[0] = x;
  values[1] = y;
  xcb_configure_window(rendering_context->canvas->connection, rendering_context->canvas->window,
    XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);
}

void xcbcanvas_set_color(canvas_rendering_context_t* rendering_context, uint32_t color)
{
  xcb_connection_t* c = rendering_context->canvas->connection;
  xcb_gcontext_t gc = rendering_context->canvas->gc;
  xcb_change_gc(c, gc, XCB_GC_FOREGROUND, &color);
}

void xcbcanvas_set_stroke_width(canvas_rendering_context_t* rendering_context, int width)
{
  xcb_connection_t* c = rendering_context->canvas->connection;
  xcb_gcontext_t gc = rendering_context->canvas->gc;
  xcb_change_gc(c, gc, XCB_GC_LINE_WIDTH, &width);
}

void xcbcanvas_stroke_rectangle(canvas_rendering_context_t* rendering_context, int16_t x, int16_t y, uint16_t width, uint16_t height)
{
  xcb_connection_t* c = rendering_context->canvas->connection;
  xcb_gcontext_t gc = rendering_context->canvas->gc;
  xcb_rectangle_t rectangle;
  rectangle.x = x;
  rectangle.y = y;
  rectangle.width = width;
  rectangle.height = height;
  xcb_poly_rectangle(c, rendering_context->canvas->window, gc, 1, &rectangle);
}

void xcbcanvas_fill_rectangle(canvas_rendering_context_t* rendering_context, int16_t x, int16_t y, uint16_t width, uint16_t height)
{
  xcb_connection_t* c = rendering_context->canvas->connection;
  xcb_gcontext_t gc = rendering_context->canvas->gc;
  xcb_rectangle_t rectangle;
  rectangle.x = x;
  rectangle.y = y;
  rectangle.width = width;
  rectangle.height = height;
  xcb_poly_fill_rectangle(c, rendering_context->canvas->window, gc, 1, &rectangle);
}

void xcbcanvas_draw_text(canvas_rendering_context_t* rendering_context, int16_t x, int16_t y, const char* text)
{
  xcb_void_cookie_t       cookie_gc;
  xcb_void_cookie_t       cookie_text;
  xcb_generic_error_t* error;
  xcb_gcontext_t          gc;
  uint8_t                 length;
  length = strlen(text);
  gc = gc_font_get(rendering_context, "7x13");
  cookie_text = xcb_image_text_8_checked(
    rendering_context->canvas->connection,
    length,
    rendering_context->canvas->window,
    rendering_context->canvas->gc,
    x,
    y,
    text
  );
  error = xcb_request_check(rendering_context->canvas->connection, cookie_text);
  if (error) {
    fprintf(stderr, "Error: Can't paste text: %d\n", error->error_code);
    free(error);
  }
}

void xcbcanvas_line(canvas_rendering_context_t* rendering_context, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
  xcb_connection_t* c = rendering_context->canvas->connection;
  xcb_gcontext_t gc = rendering_context->canvas->gc;
  xcb_point_t points[2];
  points[0].x = x1;
  points[0].y = y1;
  points[1].x = x2;
  points[1].y = y2;
  xcb_poly_line(c, XCB_COORD_MODE_ORIGIN, rendering_context->canvas->window, gc, 2, points);
}

void xcbcanvas_arc(canvas_rendering_context_t* rendering_context, int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t angle1, uint16_t angle2)
{
  xcb_connection_t* c = rendering_context->canvas->connection;
  xcb_gcontext_t gc = rendering_context->canvas->gc;
  xcb_arc_t arc;
  arc.x = x;
  arc.y = y;
  arc.width = width;
  arc.height = height;
  arc.angle1 = angle1;
  arc.angle2 = angle2;
  xcb_poly_arc(c, rendering_context->canvas->window, gc, 1, &arc);
}

void xcbcanvas_fill_arc(canvas_rendering_context_t* rendering_context, int16_t x, int16_t y, uint16_t width, uint16_t height, uint16_t angle1, uint16_t angle2)
{
  xcb_connection_t* c = rendering_context->canvas->connection;
  xcb_gcontext_t gc = rendering_context->canvas->gc;
  xcb_arc_t arc;
  arc.x = x;
  arc.y = y;
  arc.width = width;
  arc.height = height;
  arc.angle1 = angle1;
  arc.angle2 = angle2;
  xcb_poly_fill_arc(c, rendering_context->canvas->window, gc, 1, &arc);
}

//#define XCBCANVAS_DEBUG_DRAW_PATH // uncomment to debug path drawing
void xcbcanvas_draw_path(canvas_rendering_context_t* rendering_context)
{

  if (rendering_context->path->sub_path_count == 0) {
    printf("Error: No sub-paths in path\n");
    return;
  }

  /*  Technically valid, but would do nothing
      since the first sub-path is treated as just a move to.
  */
  if (rendering_context->path->sub_path_count == 1) return;

  /* Render a outline path */
  xcb_point_t current_position = rendering_context->path->sub_paths[0].point;
  xcb_point_t closing_point = rendering_context->path->sub_paths[0].point;
  xcb_point_t points[rendering_context->path->sub_path_count + 1];
  points[0] = current_position;
  uint16_t moves_since_close = 0;
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
  printf("Path: %d sub-paths\n", rendering_context->path->sub_path_count);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
  if (!rendering_context->path->filled) {
    for (int i = 1; i < rendering_context->path->sub_path_count; i++) {
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
      printf("Current position: %d, %d\n", current_position.x, current_position.y);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
      switch (rendering_context->path->sub_paths[i].type) {
        case SUBPATH_TYPE_MOVE:
          current_position = rendering_context->path->sub_paths[i].point;
          if (moves_since_close > 1) points[moves_since_close + 1] = closing_point;
          moves_since_close = 0;
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
          printf("#%d: Move to: %d, %d\n", i, current_position.x, current_position.y);

#endif // XCBCANVAS_DEBUG_DRAW_PATH
          break;
        case SUBPATH_TYPE_LINE:
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
          printf("#%d: Line to: %d, %d\n", i, rendering_context->path->sub_paths[i].point.x, rendering_context->path->sub_paths[i].point.y);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
          xcbcanvas_line(rendering_context, current_position.x, current_position.y, rendering_context->path->sub_paths[i].point.x, rendering_context->path->sub_paths[i].point.y);
          current_position = rendering_context->path->sub_paths[i].point;
          points[++moves_since_close] = current_position;
          break;
        case SUBPATH_TYPE_ARC:
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
          printf("#%d: Arc: %d, %d, %d, %d, %d, %d\n",
            i,
            rendering_context->path->sub_paths[i].point.x,
            rendering_context->path->sub_paths[i].point.y,
            rendering_context->path->sub_paths[i].arc.radius,
            rendering_context->path->sub_paths[i].arc.radius,
            rendering_context->path->sub_paths[i].arc.start_radius_or_cp2_x,
            rendering_context->path->sub_paths[i].arc.end_radius_or_cp2_y);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
          xcbcanvas_arc(rendering_context,
            rendering_context->path->sub_paths[i].point.x,
            rendering_context->path->sub_paths[i].point.y,
            rendering_context->path->sub_paths[i].arc.radius,
            rendering_context->path->sub_paths[i].arc.radius,
            rendering_context->path->sub_paths[i].arc.start_radius_or_cp2_x,
            rendering_context->path->sub_paths[i].arc.end_radius_or_cp2_y);
          break;
        case SUBPATH_TYPE_ARC_TO:
        case SUBPATH_TYPE_QUADRATIC_CURVE:
        case SUBPATH_TYPE_CUBIC_CURVE:
          printf("Error: Unsupported path sub-path type: %d\n", rendering_context->path->sub_paths[i].type);
          break;
        case SUBPATH_TYPE_CLOSE:
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
          printf("#%d: Close (%d, %d)\n", i, closing_point.x, closing_point.y);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
          if (moves_since_close > 1) points[moves_since_close + 1] = closing_point;
          xcbcanvas_line(rendering_context, current_position.x, current_position.y, closing_point.x, closing_point.y);
          current_position = rendering_context->path->sub_paths[0].point;
          break;
      }
    }
  }
  /* Render a filled path */
  else {
    for (int i = 1; i < rendering_context->path->sub_path_count; i++) {
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
      printf("Current position: %d, %d\n", current_position.x, current_position.y);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
      switch (rendering_context->path->sub_paths[i].type) {
        case SUBPATH_TYPE_MOVE:
          if (moves_since_close > 1) {
            points[++moves_since_close] = closing_point;
            xcb_fill_poly(
              rendering_context->canvas->connection,
              rendering_context->canvas->window,
              rendering_context->canvas->gc,
              XCB_POLY_SHAPE_COMPLEX,
              XCB_COORD_MODE_ORIGIN,
              moves_since_close,
              points);
          }
          moves_since_close = 0;
          current_position = rendering_context->path->sub_paths[i].point;
          closing_point = current_position;
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
          printf("#%d: Move to: %d, %d\n", i, rendering_context->path->sub_paths[i].point.x, rendering_context->path->sub_paths[i].point.y);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
          break;
        case SUBPATH_TYPE_LINE:
          current_position = rendering_context->path->sub_paths[i].point;
          points[++moves_since_close] = current_position;
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
          printf("#%d:\tLine to: %d, %d\n", i, rendering_context->path->sub_paths[i].point.x, rendering_context->path->sub_paths[i].point.y);
          printf("\tMoves since close: %d\n", moves_since_close);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
          break;
        case SUBPATH_TYPE_ARC:
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
          printf("#%d: Arc: %d, %d, %d, %d, %d, %d\n",
            i,
            rendering_context->path->sub_paths[i].point.x,
            rendering_context->path->sub_paths[i].point.y,
            rendering_context->path->sub_paths[i].arc.radius,
            rendering_context->path->sub_paths[i].arc.radius,
            rendering_context->path->sub_paths[i].arc.start_radius_or_cp2_x,
            rendering_context->path->sub_paths[i].arc.end_radius_or_cp2_y);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
          xcbcanvas_fill_arc(rendering_context,
            rendering_context->path->sub_paths[i].point.x,
            rendering_context->path->sub_paths[i].point.y,
            rendering_context->path->sub_paths[i].arc.radius,
            rendering_context->path->sub_paths[i].arc.radius,
            rendering_context->path->sub_paths[i].arc.start_radius_or_cp2_x,
            rendering_context->path->sub_paths[i].arc.end_radius_or_cp2_y
          );
          break;
        case SUBPATH_TYPE_ARC_TO:
        case SUBPATH_TYPE_QUADRATIC_CURVE:
        case SUBPATH_TYPE_CUBIC_CURVE:
          printf("Error: Unsupported path sub-path type: %d\n", rendering_context->path->sub_paths[i].type);
          break;
        case SUBPATH_TYPE_CLOSE:
          if (moves_since_close > 1) {
            points[++moves_since_close] = closing_point;
            xcb_fill_poly(
              rendering_context->canvas->connection,
              rendering_context->canvas->window,
              rendering_context->canvas->gc,
              XCB_POLY_SHAPE_COMPLEX,
              XCB_COORD_MODE_ORIGIN,
              moves_since_close,
              points);
          }
          current_position = rendering_context->path->sub_paths[0].point;
          moves_since_close = 0;
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
          printf("#%d: Close (%d, %d)\n", i, closing_point.x, closing_point.y);
          for (int j = 0; j < moves_since_close; j++) {
            printf("  Point %d: (%d, %d)\n", j, points[j].x, points[j].y);
          }
#endif // XCBCANVAS_DEBUG_DRAW_PATH
          break;
      }
    }
  }

  xcb_flush(rendering_context->canvas->connection);
}