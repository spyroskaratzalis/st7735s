#ifndef ST7735S_H
#define ST7735S_H

/**
 * @file st7735s.h
 * @brief Master umbrella header for the ST7735S Linux / Raspberry Pi driver library.
 *
 * This library provides a lightweight, performant C11 driver for the Sitronix ST7735S
 * LCD controller (specifically tuned for the Waveshare 1.44-inch 128x128 SPI display HAT)
 * running on modern Linux kernels using `lgpio`.
 *
 * Modules:
 * - display.h:  Hardware lifecycle, SPI communication, in-memory RGB565 framebuffer,
 *               and display flushing (display_present).
 * - graphics.h: Primitive rasterization (Bresenham lines, rectangles, circles, bitmaps).
 * - image.h:    Asset decoders (uncompressed 24-bit Windows BMP and animated GIF via stb_image).
 * - input.h:    GPIO input reader for the 5-way joystick and 3 user keys with edge detection.
 */

#include "st7735s/display.h"
#include "st7735s/graphics.h"
#include "st7735s/image.h"
#include "st7735s/input.h"

#endif // ST7735S_H