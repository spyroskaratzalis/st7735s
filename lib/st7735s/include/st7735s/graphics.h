#ifndef ST7735S_GRAPHICS_H
#define ST7735S_GRAPHICS_H

#include <stdbool.h>
#include <stdint.h>

#include "st7735s/image.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Draws a fast horizontal line.
 *
 * @param x Starting X coordinate (leftmost point).
 * @param y Y coordinate of the row.
 * @param w Width of the line in pixels. If w <= 0, nothing is drawn.
 * @param color 16-bit RGB565 color.
 */
void graphics_draw_fast_hline(int x, int y, int w, uint16_t color);

/**
 * Draws a fast vertical line.
 *
 * @param x X coordinate of the column.
 * @param y Starting Y coordinate (topmost point).
 * @param h Height of the line in pixels. If h <= 0, nothing is drawn.
 * @param color 16-bit RGB565 color.
 */
void graphics_draw_fast_vline(int x, int y, int h, uint16_t color);

/**
 * Draws a 1-pixel wide line between two arbitrary points using Bresenham's algorithm.
 *
 * Fully supports all octants, steep slopes, horizontal/vertical lines, and
 * inverted coordinates (x0 > x1 or y0 > y1). Coordinates falling outside
 * display boundaries are clipped automatically per-pixel.
 *
 * @param x0 Starting X coordinate.
 * @param y0 Starting Y coordinate.
 * @param x1 Ending X coordinate.
 * @param y1 Ending Y coordinate.
 * @param color 16-bit RGB565 color.
 */
void graphics_draw_line(int x0, int y0, int x1, int y1, uint16_t color);

/**
 * Draws an unfilled 1-pixel outline of a rectangle.
 *
 * @param x Top-left X coordinate.
 * @param y Top-left Y coordinate.
 * @param w Width in pixels (extends from x to x + w - 1).
 * @param h Height in pixels (extends from y to y + h - 1).
 * @param color 16-bit RGB565 color.
 */
void graphics_draw_rect(int x, int y, int w, int h, uint16_t color);

/**
 * Draws a solid filled rectangle.
 *
 * @param x Top-left X coordinate.
 * @param y Top-left Y coordinate.
 * @param w Width in pixels.
 * @param h Height in pixels.
 * @param color 16-bit RGB565 color.
 */
void graphics_fill_rect(int x, int y, int w, int h, uint16_t color);

/**
 * Draws an unfilled 1-pixel circle outline using the Midpoint Circle algorithm.
 *
 * @param cx Center X coordinate.
 * @param cy Center Y coordinate.
 * @param r Radius in pixels. If r <= 0, draws a single pixel or returns.
 * @param color 16-bit RGB565 color.
 */
void graphics_draw_circle(int cx, int cy, int r, uint16_t color);

/**
 * Draws a solid filled circle.
 *
 * @param cx Center X coordinate.
 * @param cy Center Y coordinate.
 * @param r Radius in pixels.
 * @param color 16-bit RGB565 color.
 */
void graphics_fill_circle(int cx, int cy, int r, uint16_t color);

/**
 * Blits a raw contiguous RGB565 buffer onto the framebuffer with rectangular clipping.
 *
 * Useful for custom sprite sheets or procedurally generated bitmap buffers.
 *
 * @param x Top-left destination X coordinate on display.
 * @param y Top-left destination Y coordinate on display.
 * @param w Width of the source bitmap in pixels.
 * @param h Height of the source bitmap in pixels.
 * @param pixels Pointer to array of w * h RGB565 pixels (row-major order).
 */
void graphics_draw_bitmap(int x, int y, int w, int h, const uint16_t *pixels);

/**
 * Blits a decoded Image struct onto the framebuffer with rectangular clipping.
 *
 * Coordinates may be negative or partially off-screen; off-screen pixels are
 * discarded without memory corruption.
 *
 * @param x Top-left destination X coordinate on display.
 * @param y Top-left destination Y coordinate on display.
 * @param img Pointer to the Image containing width, height, and pixel buffer.
 */
void graphics_draw_image(int x, int y, const Image* img);

#ifdef __cplusplus
}
#endif

#endif  // ST7735S_GRAPHICS_H