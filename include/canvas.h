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
 * @param width                Width of the rectangle.
 * @param height               Height of the rectangle.
*/
void canvas_fill_rectangle(
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
void canvas_draw_text(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    const char* text
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

#endif /* CANVAS_H */
