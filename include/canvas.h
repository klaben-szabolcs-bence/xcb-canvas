#ifndef CANVAS_H
#define CANVAS_H

#include "xcb-canvas.h"

/**
 * @brief Initializes the canvas.
 *
 * @param rendering_context The rendering context to use.
 * @return -1 on failure, 0 on success.
*/
int canvas_init(canvas_rendering_context_t* rendering_context);

/**
 * @brief Deallocates the canvas.
 *
 * @param rendering_context The rendering context to use.
*/
void canvas_dealloc(canvas_rendering_context_t* rendering_context);

/**
 * @brief Sets the drawing function.
 *
 * @param rendering_context The rendering context to use.
 * @param draw_function     The drawing function to use.
 */
void canvas_set_draw_function(canvas_rendering_context_t* rendering_context, void (*draw_function) ());

/* TODO: Add "path" support from Canvas. */

/**
 * @brief Draws a rectangle outline.
 *
 * @param rendering_context    The rendering context.
 * @param x                    Top left x coordinate.
 * @param y                    Top left y coordinate.
 * @param width                Width of the rectangle.
 * @param height               Height of the rectangle.
*/
void canvas_stroke_rectangle(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    uint16_t width, uint16_t height
);

/**
 * @brief Draws a filled rectangle.
 *
 * @param rendering_context    The rendering context used.
 * @param x                    Top left y coordinate of the rectangle.
 * @param y                    Top left x coordinate of the rectangle.
 * @param width                Width of the rectangle.
 * @param height               Height of the rectangle.
*/
void canvas_fill_rectangle(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    uint16_t width, uint16_t height
);

/**
 * @brief Draws a rectangle in white.
 *
 * @param rendering_context     The rendering context used.
 * @param x                     Top left x coordinate of the rectangle.
 * @param y                     Top left y coordinate of the rectangle.
 * @param width                 Width of the rectangle.
 * @param height                Height of the rectangle.
 */
void canvas_clear_rectangle(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    uint16_t width, uint16_t height
);

/**
 * @brief Draws text on the canvas.
 *
 * @param rendering_context The rendering context used.
 * @param x                 The x coordinate of the text.
 * @param y                 The y coordinate (the baseline) of the text.
 * @param text              The text to draw.
 */
void canvas_fill_text(
    canvas_rendering_context_t* rendering_context,
    const char* text,
    int16_t x, int16_t y
);

/**
 * @brief Updates the font used for text drawing.
 *
 * @param rendering_context The rendering context used.
 * @param family            The font family to use. Leave NULL to not change.
 * @param size              The font size to use. Leave 0 to not change.
 * @param weight            The font weight to use.
 * Font weight should be given as a string. Example: "Bold" or "Medium". Leave NULL to not change.
 * @param italic            Whether the font should be italic. Will always change.
 */
void canvas_font4(
    canvas_rendering_context_t* rendering_context,
    const char* family,
    uint16_t size,
    const char* weight,
    _Bool italic
);

/**
 * @brief Updates the font used for text drawing.
 * Weight will be set to Medium and it will be non-italic.
 *
 * @param rendering_context The rendering context used.
 * @param family            The font family to use. Leave NULL to not change.
 * @param size              The font size to use. Leave 0 to not change.
 */
void canvas_font2(
    canvas_rendering_context_t* rendering_context,
    const char* family,
    uint16_t size
);

/**
 * @brief Sets the color of the rendering context.
 *
 * @param rendering_context The rendering context to use.
 * @param red               The red component of the color.
 * @param green             The green component of the color.
 * @param blue              The blue component of the color.
 */
void canvas_set_color(
    canvas_rendering_context_t* rendering_context,
    uint8_t red, uint8_t green, uint8_t blue
);

/**
 * @brief Sets the width of outlines drawn on the canvas.
 *
 * @param rendering_context The rendering context to use.
 * @param width             The width of the outlines.
 */
void canvas_set_line_width(
    canvas_rendering_context_t* rendering_context,
    uint16_t width
);

/**
 * @brief Begins a new path.
 *
 * @param rendering_context The rendering context to use.
 */
void canvas_begin_path(canvas_rendering_context_t* rendering_context);

/**
 * @brief Moves the pen of the path to the given position.
 *
 * @param rendering_context The rendering context to use.
 * @param x                 The x coordinate of the position.
 * @param y                 The y coordinate of the position.
 */
void canvas_move_to(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y
);

/**
 * @brief Draws a line from the current pen position to the given position.
 *
 * @param rendering_context The rendering context to use.
 * @param x                 The x coordinate of the position.
 * @param y                 The y coordinate of the position.
 */
void canvas_line_to(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y
);

/**
 * @brief Draws an arc from the current pen position to the given position.
 *
 * @param rendering_context The rendering context to use.
 * @param x                 The x coordinate of the position.
 * @param y                 The y coordinate of the position.
 * @param radius            The radius of the arc.
 * @param start_angle       The start angle of the arc in 1/64 of a degree.
 * @param end_angle         The end angle of the arc in 1/64 of a degree.
 * @param clockwise     Whether the arc should be drawn in clockwise.
 *
 * @note To get the correct angles, use the following formula: angle << 6.
 */
void canvas_arc(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    uint16_t radius,
    uint16_t start_angle, uint16_t end_angle,
    _Bool clockwise
);

/**
 * @brief Draws an arc with the tangent lines from the current pen position to the first position,
 *  and from the first to the second position with the given radius.
 *
 * @param rendering_context The rendering context to use.
 * @param x1                The x coordinate of the first position.
 * @param y1                The y coordinate of the first position.
 * @param x2                The x coordinate of the second position.
 * @param y2                The y coordinate of the second position.
 * @param radius            The radius of the arc.
 */
void canvas_arc_to(
    canvas_rendering_context_t* rendering_context,
    int16_t x1, int16_t y1,
    int16_t x2, int16_t y2,
    uint16_t radius
);

/**
 * @brief Closes the current path.
 *
 * @param rendering_context The rendering context to use.
 */
void canvas_close_path(canvas_rendering_context_t* rendering_context);

/**
 * @brief Strokes the current path.
 *
 * @param rendering_context The rendering context to use.
 */
void canvas_stroke(canvas_rendering_context_t* rendering_context);

/**
 * @brief Fills the current path.
 *
 * @param rendering_context The rendering context to use.
 */
void canvas_fill(canvas_rendering_context_t* rendering_context);

#endif /* CANVAS_H */
