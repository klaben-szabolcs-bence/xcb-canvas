#ifndef CANVAS_H
#define CANVAS_H

#include "xcb-canvas.h"
#include <string.h>

/**
 * @brief Initializes the canvas.
 * 
 * @param rendering_context The rendering context to use.
*/
void canvas_init(canvas_rendering_context_t* rendering_context);

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

#endif /* CANVAS_H */
