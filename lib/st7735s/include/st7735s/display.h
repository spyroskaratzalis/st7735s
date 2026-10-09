#ifndef ST7735S_DISPLAY_H
#define ST7735S_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_WIDTH   128
#define LCD_HEIGHT  128

// Standard RGB565 Color Definitions (MSB: RRRRRGGG, LSB: GGGBBBBB)
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
 * @brief Initializes GPIOs, Hardware SPI, runs display reset and initialization sequence.
 * @return 0 on success, -1 on failure.
 */
int display_init(void);

/**
 * @brief Turns off backlight, releases SPI device, and closes GPIO handles.
 */
void display_close(void);

/**
 * @brief Fills the entire in-memory framebuffer with a single color.
 * @param color 16-bit RGB565 color value.
 */
void display_clear(uint16_t color);

/**
 * @brief Draws a single pixel into the in-memory framebuffer with boundary clipping.
 * @param x Horizontal pixel position (0 to LCD_WIDTH - 1).
 * @param y Vertical pixel position (0 to LCD_HEIGHT - 1).
 * @param color 16-bit RGB565 color value.
 */
void display_draw_pixel(int x, int y, uint16_t color);

/**
 * @brief Flushes the local framebuffer over SPI to the physical ST7735S screen.
 */
void display_present(void);

/**
 * @brief Enables or disables the display backlight.
 * @param enable true turns backlight ON, false turns it OFF.
 */
void display_set_backlight(bool enable);

#ifdef __cplusplus
}
#endif

#endif // ST7735S_DISPLAY_H