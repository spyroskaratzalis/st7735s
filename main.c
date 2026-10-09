#include <stdio.h>
#include <unistd.h>
#include "st7735s/display.h"
#include "st7735s/graphics.h"

int main(void)
{
    if (display_init() != 0) {
        fprintf(stderr, "Failed to initialize display\n");
        return 1;
    }

    // 1. Clear background
    display_clear(COLOR_BLACK);

    // 2. Outer boundary border
    graphics_draw_rect(0, 0, LCD_WIDTH, LCD_HEIGHT, COLOR_WHITE);

    // 3. Diagonal corner-to-corner X (Bresenham)
    graphics_draw_line(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, COLOR_GRAY);
    graphics_draw_line(0, LCD_HEIGHT - 1, LCD_WIDTH - 1, 0, COLOR_GRAY);

    // 4. Rectangles: Outline (Cyan) and Solid Fill (Magenta)
    graphics_draw_rect(10, 10, 30, 20, COLOR_CYAN);
    graphics_fill_rect(14, 14, 22, 12, COLOR_MAGENTA);

    // 5. Rectangles on the right: Outline (Yellow) and Solid Fill (Red)
    graphics_draw_rect(88, 10, 30, 20, COLOR_YELLOW);
    graphics_fill_rect(92, 14, 22, 12, COLOR_RED);

    // 6. Circles: Concentric wireframes (Green, Blue)
    graphics_draw_circle(64, 64, 40, COLOR_GREEN);
    graphics_draw_circle(64, 64, 25, COLOR_BLUE);

    // 7. Center target: Filled circle (Yellow) with Red center core
    graphics_fill_circle(64, 64, 12, COLOR_YELLOW);
    graphics_fill_circle(64, 64, 4, COLOR_RED);

    // 8. Horizontal & vertical crosshairs through center
    graphics_draw_fast_hline(44, 64, 40, COLOR_WHITE);
    graphics_draw_fast_vline(64, 44, 40, COLOR_WHITE);

    // Commit buffer to SPI
    display_present();

    printf("Test pattern displayed. Holding for 5 seconds...\n");
    sleep(5);

    display_close();
    return 0;
}