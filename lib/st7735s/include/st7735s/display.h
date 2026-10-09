#ifndef ST7735S_DISPLAY_H
#define ST7735S_DISPLAY_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_WIDTH   128
#define LCD_HEIGHT  128

// Common RGB565 colors (High byte first: RRRRRGGG GGGBBBBB)
#define COLOR_BLACK   0x0000
#define COLOR_WHITE   0xFFFF
#define COLOR_RED     0x00F8
#define COLOR_GREEN   0xE007
#define COLOR_BLUE    0x1F00
#define COLOR_YELLOW  0xFFE0

/**
 * @brief Initialize SPI, reset pins, backlight, and run ST7735S init sequence.
 * @return 0 on success, negative value on failure.
 */
int display_init(void);

/**
 * @brief Shut down SPI, turn off backlight, and release GPIO handles.
 */
void display_close(void);

/**
 * @brief Fill the entire in-memory framebuffer with a single color.
 */
void display_clear(uint16_t color);

/**
 * @brief Set a single pixel in the in-memory framebuffer.
 */
void display_draw_pixel(int x, int y, uint16_t color);

/**
 * @brief Send the entire 128x128 framebuffer to the display over SPI.
 */
void display_present(void);

/**
 * @brief Toggle or dim the display backlight.
 * @param enable true to turn on, false to turn off.
 */
void display_set_backlight(bool enable);

#ifdef __cplusplus
}
#endif

#endif // ST7735S_DISPLAY_H