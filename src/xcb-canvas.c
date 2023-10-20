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
  int16_t start_radius_or_cp2_x;
  int16_t end_radius_or_cp2_y;
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

//#define XCBCANVAS_INIT_XCB_DEBUG //uncomment to debug the function below
xcbcanvas_t* xcbcanvas_init_xcb()
{
  xcb_connection_t* c;
  xcb_screen_t* screen;
  xcb_window_t win;
  xcb_gcontext_t gc;
  uint32_t mask = 0;
  uint32_t values[3];

  /* Open the connection to the X server */
  c = xcb_connect(NULL, NULL);

  if (xcb_connection_has_error(c))
  {
    printf("Error: Can't open display: %s\n", getenv("DISPLAY"));
    exit(-1);
  }

#ifdef XCBCANVAS_INIT_XCB_DEBUG
  printf("Connected to the X server\n");
#endif

  /* Get the first screen */
  screen = xcb_setup_roots_iterator(xcb_get_setup(c)).data;

  if (screen == NULL)
  {
    printf("Error: Can't get screen\n");
    exit(-1);
  }

#ifdef XCBCANVAS_INIT_XCB_DEBUG
  printf("Screen: %d\n", screen->root);
#endif

  /* Create black (foreground) and white (background) colors */
  gc = xcb_generate_id(c);
  mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND | XCB_GC_GRAPHICS_EXPOSURES;
  values[0] = screen->black_pixel;
  values[1] = screen->white_pixel;
  values[2] = 0;
  xcb_create_gc(c, gc, screen->root, mask, values);

  /* Ask for our window's Id */
  win = xcb_generate_id(c);

  if (win == 0)
  {
    printf("Error: Can't get window id\n");
    exit(-1);
  }

#ifdef XCBCANVAS_INIT_XCB_DEBUG
  printf("Window id: %d\n", win);
#endif

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
    exit(-1);
  }

#ifdef XCBCANVAS_INIT_XCB_DEBUG
  printf("Window created\n");
#endif

  xcb_void_cookie_t cookie = xcb_change_property_checked(c,
    XCB_PROP_MODE_REPLACE, win, XCB_ATOM_WM_NAME, XCB_ATOM_STRING, 8,
    strlen("XCB Canvas"), "XCB Canvas");
  xcb_generic_error_t* error = xcb_request_check(c, cookie);
  if (error)
  {
    fprintf(stderr, "Error changing window title: %d\n", error->error_code);
    free(error);
    exit(-1);
  }

#ifdef XCBCANVAS_INIT_XCB_DEBUG
  printf("Window title changed\n");
#endif

  /* Map the window on the screen */
  xcb_map_window(c, win);

  if (xcb_connection_has_error(c))
  {
    printf("Error: Can't map window:\n");
    exit(-1);
  }

#ifdef XCBCANVAS_INIT_XCB_DEBUG
  printf("Window mapped\n");
#endif

  xcb_flush(c);

#ifdef XCBCANVAS_INIT_XCB_DEBUG
  printf("Flushed\n");
#endif

  xcbcanvas_t* xcbcanvas = malloc(sizeof(xcbcanvas_t));
  xcbcanvas->connection = c;
  xcbcanvas->screen = screen;
  xcbcanvas->window = win;
  xcbcanvas->gc = gc;

#ifdef XCBCANVAS_INIT_XCB_DEBUG
  printf("XCB canvas initialized\n");
#endif

  return xcbcanvas;
}

void xcbcanvas_handle_events(
  canvas_rendering_context_t* rendering_context,
  xcbcanvas_t* canvas,
  void (*draw_function) (canvas_rendering_context_t* rendering_context)
)
{
  xcb_connection_t* c = canvas->connection;
  xcb_generic_event_t* e;
  while ((e = xcb_wait_for_event(c)))
  {
    switch (e->response_type & ~0x80)
    {
      case XCB_EXPOSE:
      {
        xcb_expose_event_t* ev = (xcb_expose_event_t*)e;
        draw_function(rendering_context);
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

void xcbcanvas_set_window_size(xcbcanvas_t* canvas, int new_width, int new_height)
{
  static uint32_t values[2];
  values[0] = new_width;
  values[1] = new_height;
  xcb_configure_window(canvas->connection, canvas->window,
    XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
}

xcbcanvas_size_t xcbcanvas_get_window_size(xcbcanvas_t* canvas)
{
  xcb_get_geometry_cookie_t cookie = xcb_get_geometry(canvas->connection, canvas->window);
  xcb_get_geometry_reply_t* reply = xcb_get_geometry_reply(canvas->connection, cookie, NULL);
  xcbcanvas_size_t size;
  size.width = reply->width;
  size.height = reply->height;
  free(reply);
  return size;
}

void xcbcanvas_load_font(xcbcanvas_t* canvas, char* font_name)
{
  // First, create an id for the font
  xcb_font_t font = xcb_generate_id(canvas->connection);
  // Secondly, open the font
  xcb_void_cookie_t cookie = xcb_open_font_checked(canvas->connection, font, strlen(font_name), font_name);
  // Check for errors
  xcb_generic_error_t* error = xcb_request_check(canvas->connection, cookie);
  if (error)
  {
    fprintf(stderr, "Error opening font: %s\n", font_name);
    free(error);
    return;
  }

  // Then, assign font to our existing graphic context
  cookie = xcb_change_gc(canvas->connection, canvas->gc, XCB_GC_FONT, (uint32_t[]) { font });
  // Check for errors
  error = xcb_request_check(canvas->connection, cookie);
  if (error)
  {
    fprintf(stderr, "Error assigning font to graphic context\n");
    free(error);
    return;
  }
  // Close the font
  cookie = xcb_close_font_checked(canvas->connection, font);
  error = xcb_request_check(canvas->connection, cookie);
  if (error)
  {
    fprintf(stderr, "Error closing font\n");
    free(error);
    return;
  }
}

void xcbcanvas_set_window_title(xcbcanvas_t* canvas, char* title)
{
  xcb_connection_t* c = canvas->connection;
  xcb_window_t win = canvas->window;
  xcb_change_property(c, XCB_PROP_MODE_REPLACE, win, XCB_ATOM_WM_NAME,
    XCB_ATOM_STRING, 8, strlen(title), title);
}

void xcbcanvas_set_window_position(xcbcanvas_t* canvas, int x, int y)
{
  static uint32_t values[2];
  values[0] = x;
  values[1] = y;
  xcb_configure_window(canvas->connection, canvas->window,
    XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);
}

void xcbcanvas_set_color(xcbcanvas_t* canvas, uint32_t color)
{
  xcb_connection_t* c = canvas->connection;
  xcb_gcontext_t gc = canvas->gc;
  xcb_change_gc(c, gc, XCB_GC_FOREGROUND, &color);
}

void xcbcanvas_set_stroke_width(xcbcanvas_t* canvas, int width)
{
  xcb_connection_t* c = canvas->connection;
  xcb_gcontext_t gc = canvas->gc;
  xcb_change_gc(c, gc, XCB_GC_LINE_WIDTH, &width);
}

void xcbcanvas_stroke_rectangle(xcbcanvas_t* canvas, int16_t x, int16_t y, uint16_t width, uint16_t height)
{
  xcb_connection_t* c = canvas->connection;
  xcb_gcontext_t gc = canvas->gc;
  xcb_rectangle_t rectangle;
  rectangle.x = x;
  rectangle.y = y;
  rectangle.width = width;
  rectangle.height = height;
  xcb_poly_rectangle(c, canvas->window, gc, 1, &rectangle);
}

void xcbcanvas_fill_rectangle(xcbcanvas_t* canvas, int16_t x, int16_t y, uint16_t width, uint16_t height)
{
  xcb_connection_t* c = canvas->connection;
  xcb_gcontext_t gc = canvas->gc;
  xcb_rectangle_t rectangle;
  rectangle.x = x;
  rectangle.y = y;
  rectangle.width = width;
  rectangle.height = height;
  xcb_poly_fill_rectangle(c, canvas->window, gc, 1, &rectangle);
}

void xcbcanvas_draw_text(xcbcanvas_t* canvas, int16_t x, int16_t y, const char* text)
{
  // Draw the text to the screen with the font that has been set
  xcb_void_cookie_t cookie_text = xcb_image_text_8_checked(
    canvas->connection,
    strlen(text),
    canvas->window,
    canvas->gc,
    x,
    y,
    text
  );
  xcb_generic_error_t* error = xcb_request_check(canvas->connection, cookie_text);
  if (error) {
    fprintf(stderr, "Error: Can't paste text: %d\n", error->error_code);
    free(error);
  }
}

void xcbcanvas_line(xcbcanvas_t* canvas, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
  xcb_connection_t* c = canvas->connection;
  xcb_gcontext_t gc = canvas->gc;
  xcb_point_t points[2];
  points[0].x = x1;
  points[0].y = y1;
  points[1].x = x2;
  points[1].y = y2;
  xcb_poly_line(c, XCB_COORD_MODE_ORIGIN, canvas->window, gc, 2, points);
}

void xcbcanvas_arc(xcbcanvas_t* canvas, int16_t x, int16_t y, uint16_t width, uint16_t height, int16_t angle1, int16_t angle2)
{
  xcb_connection_t* c = canvas->connection;
  xcb_gcontext_t gc = canvas->gc;
  xcb_arc_t arc;
  arc.x = x - width / 2;
  arc.y = y - height / 2;
  arc.width = width;
  arc.height = height;
  arc.angle1 = angle1;
  arc.angle2 = angle2;
  xcb_poly_arc(c, canvas->window, gc, 1, &arc);
}

void xcbcanvas_fill_arc(xcbcanvas_t* canvas, int16_t x, int16_t y, uint16_t width, uint16_t height, int16_t angle1, int16_t angle2)
{
  xcb_connection_t* c = canvas->connection;
  xcb_gcontext_t gc = canvas->gc;
  xcb_arc_t arc;
  arc.x = x - width / 2;
  arc.y = y - height / 2;
  arc.width = width;
  arc.height = height;
  arc.angle1 = angle1;
  arc.angle2 = angle2;
  xcb_poly_fill_arc(c, canvas->window, gc, 1, &arc);
}

//#define XCBCANVAS_DEBUG_DRAW_PATH // uncomment to debug path drawing
void xcbcanvas_draw_path(xcbcanvas_t* canvas, path_t* path)
{

  if (path == NULL) {
    printf("Error: Canvas path is NULL\n");
    return;
  }

  if (path->sub_paths == NULL) {
    printf("Error: Sub-paths is NULL\n");
    return;
  }

  if (path->sub_path_count == 0) {
    printf("Error: No sub-paths in path\n");
    return;
  }

  /*  Technically valid, but would do nothing
      since the first sub-path is treated as just a move to.
  */
  if (path->sub_path_count == 1) {
    printf("Warning: Only one sub-path in path\n");
    return;
  }

  /* Render a outline path */
  xcb_point_t current_position = path->sub_paths[0].point;
  xcb_point_t closing_point = path->sub_paths[0].point;
  /* Keep track of all the points that make up the shape that would need to be closed */
  xcb_point_t points[path->sub_path_count + 1];
  points[0] = current_position;
  uint16_t moves_since_close = 0;
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
  printf("Path: %d sub-paths\n", path->sub_path_count);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
  if (!path->filled) {
    for (int i = 0; i < path->sub_path_count; i++) {
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
      printf("Current position: %d, %d\n", current_position.x, current_position.y);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
      switch (path->sub_paths[i].type) {
        case SUBPATH_TYPE_MOVE:
          /* Move pen to the given position */
          current_position = path->sub_paths[i].point;
          /* Add the point to the list of points that need to be closed */
          if (moves_since_close > 1) points[moves_since_close + 1] = closing_point;
          /* Reset moves since close counter */
          moves_since_close = 0;
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
          printf("#%d: Move to: %d, %d\n", i, current_position.x, current_position.y);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
          break;
        case SUBPATH_TYPE_LINE:
          /* Draw line between current position and the given one */
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
          printf("#%d: Line to: %d, %d\n", i, path->sub_paths[i].point.x, path->sub_paths[i].point.y);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
          xcbcanvas_line(canvas, current_position.x, current_position.y, path->sub_paths[i].point.x, path->sub_paths[i].point.y);
          /* Update currrent position */
          current_position = path->sub_paths[i].point;
          points[++moves_since_close] = current_position;
          break;
        case SUBPATH_TYPE_ARC:
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
          printf("#%d: Arc: %d, %d, %d, %d, %d, %d\n",
            i,
            path->sub_paths[i].point.x,
            path->sub_paths[i].point.y,
            path->sub_paths[i].arc.radius * 2,
            path->sub_paths[i].arc.radius * 2,
            path->sub_paths[i].arc.start_radius_or_cp2_x << 6,
            path->sub_paths[i].arc.end_radius_or_cp2_y << 6);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
          xcbcanvas_arc(canvas,
            path->sub_paths[i].point.x,
            path->sub_paths[i].point.y,
            path->sub_paths[i].arc.radius * 2,
            path->sub_paths[i].arc.radius * 2,
            path->sub_paths[i].arc.start_radius_or_cp2_x,
            path->sub_paths[i].arc.end_radius_or_cp2_y);
          break;
        case SUBPATH_TYPE_ARC_TO:
        case SUBPATH_TYPE_QUADRATIC_CURVE:
        case SUBPATH_TYPE_CUBIC_CURVE:
          printf("Error: Unsupported path sub-path type: %d\n", path->sub_paths[i].type);
          break;
        case SUBPATH_TYPE_CLOSE:
        /* Close the shape */
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
          printf("#%d: Close (%d, %d)\n", i, closing_point.x, closing_point.y);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
          if (moves_since_close > 1) points[moves_since_close + 1] = closing_point;
          xcbcanvas_line(canvas, current_position.x, current_position.y, closing_point.x, closing_point.y);
          current_position = path->sub_paths[0].point;
          break;
      }
    }
  }
  /* Render a filled path */
  else {
    /*  Same as above, but with a filled variants.
    *   When closing a shape, also need to make sure
    *   to correctly fill the shape.
    */
    for (int i = 0; i < path->sub_path_count; i++) {
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
      printf("Current position: %d, %d\n", current_position.x, current_position.y);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
      switch (path->sub_paths[i].type) {
        case SUBPATH_TYPE_MOVE:
          if (moves_since_close > 1) {
            points[++moves_since_close] = closing_point;
            xcb_fill_poly(
              canvas->connection,
              canvas->window,
              canvas->gc,
              XCB_POLY_SHAPE_COMPLEX,
              XCB_COORD_MODE_ORIGIN,
              moves_since_close,
              points);
          }
          moves_since_close = 0;
          current_position = path->sub_paths[i].point;
          closing_point = current_position;
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
          printf("#%d: Move to: %d, %d\n", i, path->sub_paths[i].point.x, path->sub_paths[i].point.y);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
          break;
        case SUBPATH_TYPE_LINE:
          current_position = path->sub_paths[i].point;
          points[++moves_since_close] = current_position;
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
          printf("#%d:\tLine to: %d, %d\n", i, path->sub_paths[i].point.x, path->sub_paths[i].point.y);
          printf("\tMoves since close: %d\n", moves_since_close);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
          break;
        case SUBPATH_TYPE_ARC:
#ifdef XCBCANVAS_DEBUG_DRAW_PATH
          printf("#%d: Arc: %d, %d, %d, %d, %d, %d\n",
            i,
            path->sub_paths[i].point.x,
            path->sub_paths[i].point.y,
            path->sub_paths[i].arc.radius * 2,
            path->sub_paths[i].arc.radius * 2,
            path->sub_paths[i].arc.start_radius_or_cp2_x << 6,
            path->sub_paths[i].arc.end_radius_or_cp2_y << 6);
#endif // XCBCANVAS_DEBUG_DRAW_PATH
          xcbcanvas_fill_arc(canvas,
            path->sub_paths[i].point.x,
            path->sub_paths[i].point.y,
            path->sub_paths[i].arc.radius * 2,
            path->sub_paths[i].arc.radius * 2,
            path->sub_paths[i].arc.start_radius_or_cp2_x << 6,
            path->sub_paths[i].arc.end_radius_or_cp2_y << 6
          );
          break;
        case SUBPATH_TYPE_ARC_TO:
        case SUBPATH_TYPE_QUADRATIC_CURVE:
        case SUBPATH_TYPE_CUBIC_CURVE:
          printf("Error: Unsupported path sub-path type: %d\n", path->sub_paths[i].type);
          break;
        case SUBPATH_TYPE_CLOSE:
          if (moves_since_close > 1) {
            points[++moves_since_close] = closing_point;
            xcb_fill_poly(
              canvas->connection,
              canvas->window,
              canvas->gc,
              XCB_POLY_SHAPE_COMPLEX,
              XCB_COORD_MODE_ORIGIN,
              moves_since_close,
              points);
          }
          current_position = path->sub_paths[0].point;
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
}