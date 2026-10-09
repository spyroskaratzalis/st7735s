#ifndef ST7735S_GRAPHICS_H
#define ST7735S_GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include "st7735s/image.h"

#ifdef __cplusplus
extern "C" {
#endif

// Fast axis-aligned line primitives
void graphics_draw_fast_hline(int x, int y, int w, uint16_t color);
void graphics_draw_fast_vline(int x, int y, int h, uint16_t color);

// Angled line via Bresenham's algorithm
void graphics_draw_line(int x0, int y0, int x1, int y1, uint16_t color);

// Rectangles
void graphics_draw_rect(int x, int y, int w, int h, uint16_t color);
void graphics_fill_rect(int x, int y, int w, int h, uint16_t color);

// Circles via Midpoint Circle algorithm
void graphics_draw_circle(int cx, int cy, int r, uint16_t color);
void graphics_fill_circle(int cx, int cy, int r, uint16_t color);

void graphics_draw_image(int x, int y, const Image *img);

#ifdef __cplusplus
}
#endif

#endif // ST7735S_GRAPHICS_H