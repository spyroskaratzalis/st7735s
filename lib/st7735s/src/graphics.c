#include "st7735s/graphics.h"
#include "st7735s/display.h"
#include <stdlib.h>

void graphics_draw_fast_hline(int x, int y, int w, uint16_t color) {
    if (w <= 0) return;
    for (int i = 0; i < w; ++i) {
        display_draw_pixel(x + i, y, color);
    }
}

void graphics_draw_fast_vline(int x, int y, int h, uint16_t color) {
    if (h <= 0) return;
    for (int i = 0; i < h; ++i) {
        display_draw_pixel(x, y + i, color);
    }
}

void graphics_draw_rect(int x, int y, int w, int h, uint16_t color) {
    graphics_draw_fast_hline(x, y, w, color);
    graphics_draw_fast_hline(x, y + h - 1, w, color);
    graphics_draw_fast_vline(x, y, h, color);
    graphics_draw_fast_vline(x + w - 1, y, h, color);
}

void graphics_fill_rect(int x, int y, int w, int h, uint16_t color) {
    for (int i = 0; i < h; ++i) {
        graphics_draw_fast_hline(x, y + i, w, color);
    }
}

void graphics_draw_line(int x0, int y0, int x1, int y1, uint16_t color) {
    int dx = abs(x1 - x0);
    int dy = -abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (1) {
        display_draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void graphics_draw_circle(int cx, int cy, int r, uint16_t color) {
    int x = 0;
    int y = r;
    int d = 3 - (2 * r);

    while (y >= x) {
        display_draw_pixel(cx + x, cy + y, color);
        display_draw_pixel(cx - x, cy + y, color);
        display_draw_pixel(cx + x, cy - y, color);
        display_draw_pixel(cx - x, cy - y, color);
        display_draw_pixel(cx + y, cy + x, color);
        display_draw_pixel(cx - y, cy + x, color);
        display_draw_pixel(cx + y, cy - x, color);
        display_draw_pixel(cx - y, cy - x, color);
        
        if (d <= 0) {
            d = d + (4 * x) + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void graphics_fill_circle(int cx, int cy, int r, uint16_t color)
{
    if (r < 0) return;
    if (r == 0) {
        display_draw_pixel(cx, cy, color);
        return;
    }

    int x = 0;
    int y = r;
    int d = 3 - (2 * r);

    while (y >= x) {
        // Draw horizontal spans between symmetrical pairs
        graphics_draw_fast_hline(cx - x, cy + y, 2 * x + 1, color);
        graphics_draw_fast_hline(cx - x, cy - y, 2 * x + 1, color);
        graphics_draw_fast_hline(cx - y, cy + x, 2 * y + 1, color);
        graphics_draw_fast_hline(cx - y, cy - x, 2 * y + 1, color);

        if (d <= 0) {
            d += (4 * x) + 6;
        } else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void graphics_draw_bitmap(int x, int y, int w, int h, const uint16_t *pixels)
{
    for (int row = 0; row < h; ++row) {
        int py = y + row;
        if (py < 0 || py >= LCD_HEIGHT) continue; // Row clipping

        for (int col = 0; col < w; ++col) {
            int px = x + col;
            if (px < 0 || px >= LCD_WIDTH) continue; // Col clipping

            uint16_t color = pixels[row * w + col];
            display_draw_pixel(px, py, color);
        }
    }
}

void graphics_draw_image(int x, int y, const Image *img)
{
    if (!img || !img->pixels) return;

    for (int row = 0; row < img->height; ++row) {
        int py = y + row;
        if (py < 0 || py >= LCD_HEIGHT) continue;

        for (int col = 0; col < img->width; ++col) {
            int px = x + col;
            if (px < 0 || px >= LCD_WIDTH) continue;

            display_draw_pixel(px, py, img->pixels[row * img->width + col]);
        }
    }
}