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

/** 
 * @brief Draws a point.
 * 
 * @param x                 The x coordinate of the point.
 * @param y                 The y coordinate of the point.
 * @param color             The color of the point.
 * @param rendering_context The rendering context to use.
*/
void canvas_draw_point (canvas_rendering_context_t* rendering_context, int16_t x, int16_t y);

/** 
 * @brief Draws a line.
 * 
 * @param rendering_context The rendering context.
 * @param x1                The x coordinate of the first point.
 * @param y1                The y coordinate of the first point.
 * @param x2                The x coordinate of the second point.
 * @param y2                The y coordinate of the second point.
*/
void canvas_draw_line(canvas_rendering_context_t* rendering_context, int16_t x1, int16_t y1, int16_t x2, int16_t y2);

/** 
 * @brief Draws a rectangle.
 * 
 * @param rendering_context    The rendering context.
 * @param x                    Top left x coordinate.
 * @param y                    Top left y coordinate.
 * @param width                Width of the rectangle.
 * @param height               Height of the rectangle.
*/
void canvas_draw_rectangle(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    uint16_t width, uint16_t height
);

/** 
 * @brief Draws an elliptical arc.
 * 
 * @param rendering_context    The rendering context used.
 * @param x                    Top left x coordinate of the rectangle surrounding the ellipse.
 * @param y                    Top left y coordinate of the rectangle surrounding the ellipse.
 * @param width                Width of the rectangle surrounding the ellipse.
 * @param height               Height of the rectangle surrounding the ellipse.
 * @param angle1               Starting angle of the arc.
 * @param angle2               Ending angle of the arc.
*/
void canvas_draw_arc(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    uint16_t width, uint16_t height,
    int16_t angle1, int16_t angle2
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
 * @brief Draws a filled ellipse.
 * 
 * @param rendering_context The rendering context used.
 * @param x                 Top left x coordinate of the rectangle surrounding the ellipse.
 * @param y                 Top left y coordinate of the rectangle surrounding the ellipse.
 * @param width             Width of the rectangle surrounding the ellipse.
 * @param height            Height of the rectangle surrounding the ellipse.
 * @param angle1            Starting angle of the arc.
 * @param angle2            Ending angle of the arc.
 */
void canvas_fill_arc(
    canvas_rendering_context_t* rendering_context,
    int16_t x, int16_t y,
    uint16_t width, uint16_t height,
    int16_t angle1, int16_t angle2
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
