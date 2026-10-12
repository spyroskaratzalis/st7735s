#ifndef ST7735S_DISPLAY_H
#define ST7735S_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Display dimensions in pixels for the Waveshare 1.44" ST7735S LCD.
 * Coordinate space origin (0, 0) is at the top-left corner.
 * Positive X extends rightwards [0, LCD_WIDTH - 1].
 * Positive Y extends downwards  [0, LCD_HEIGHT - 1].
 */
#define LCD_WIDTH   128
#define LCD_HEIGHT  128

/**
 * Common 16-bit RGB565 color constants.
 * Format (Big-Endian transmission):
 *   High Byte: R4 R3 R2 R1 R0 G5 G4 G3
 *   Low Byte:  G2 G1 G0 B4 B3 B2 B1 B0
 */
#define COLOR_BLACK    0x0000
#define COLOR_WHITE    0xFFFF
#define COLOR_RED      0xF800
#define COLOR_GREEN    0x07E0
#define COLOR_BLUE     0x001F
#define COLOR_YELLOW   0xFFE0
#define COLOR_CYAN     0x07FF
#define COLOR_MAGENTA  0xF81F
#define COLOR_GRAY     0x7BEF

/**
 * Initializes the ST7735S display hardware and underlying system resources.
 *
 * Execution flow:
 * 1. Opens the primary GPIO chip via lgpio (default chip 0).
 * 2. Claims DC (data/command), RST (reset), and BL (backlight) GPIO lines.
 * 3. Opens the SPI master bus (/dev/spidev0.0 at 15 MHz, Mode 0).
 * 4. Runs a hardware reset pulse (active-low toggle on RST).
 * 5. Transmits the ST7735S initialization sequence (sleep-out, frame rate,
 *    power control, gamma curves, 16-bit color mode, and display-on).
 * 6. Clears the local RAM framebuffer to black and presents it to the screen.
 * 7. Drives the backlight pin high to enable the display LED.
 *
 * Reentrancy:
 * Calling display_init() multiple times without display_close() is a safe no-op.
 *
 * @return 0 on success, or -1 if GPIO or SPI access could not be acquired
 *         (e.g., missing permissions, device busy, or driver unmapped).
 */
int display_init(void);

/**
 * Shuts down display hardware and releases OS handles.
 *
 * Drives the backlight pin low to kill power to the LED panel, closes the
 * SPI bus device, and releases all claimed GPIO lines.
 *
 * Preconditions:
 * It is safe to call display_close() even if display_init() failed midway
 * or was never called.
 */
void display_close(void);

/**
 * Fills the local in-memory framebuffer with a solid color.
 *
 * Performance note:
 * This only updates the 32 KiB local memory buffer. No SPI transactions
 * occur until display_present() is called.
 *
 * @param color 16-bit RGB565 color value.
 */
void display_clear(uint16_t color);

/**
 * Writes a single pixel into the local framebuffer with boundary clipping.
 *
 * Any coordinates outside [0, LCD_WIDTH - 1] or [0, LCD_HEIGHT - 1]
 * are silently dropped with zero overhead, making it safe for drawing
 * primitives that cross boundaries.
 *
 * @param x Horizontal pixel position (0 = left).
 * @param y Vertical pixel position (0 = top).
 * @param color 16-bit RGB565 color value.
 */
void display_draw_pixel(int x, int y, uint16_t color);

/**
 * Transmits the internal framebuffer to the ST7735S controller over SPI.
 *
 * Execution flow:
 * 1. Sets the write address window (CASET 0x2A, RASET 0x2B) including panel
 *    glass offsets (LCD_OFFSET_X = 1, LCD_OFFSET_Y = 2 for 1.44" HAT).
 * 2. Issues the RAMWR command (0x2C).
 * 3. Pulls DC high and streams the 16,384 RGB565 pixels in 1024-pixel
 *    (2048-byte) chunks across SPI to remain within Linux spidev buffer limits.
 *
 * Performance note:
 * At 15 MHz SPI, pushing a full 128x128 16-bit frame takes approximately 17-20 ms,
 * achieving roughly 50-60 FPS theoretical maximum throughput.
 */
void display_present(void);

/**
 * Controls the physical display backlight LED.
 *
 * @param enable true to turn backlight ON (active high), false to turn OFF.
 */
void display_set_backlight(bool enable);

#ifdef __cplusplus
}
#endif

#endif // ST7735S_DISPLAY_H