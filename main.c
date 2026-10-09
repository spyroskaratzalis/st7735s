#include <stdio.h>
#include <unistd.h>
#include "st7735s/display.h"

int main(void) {
    if (display_init() != 0) {
        fprintf(stderr, "Failed to initialize ST7735S display\n");
        return 1;
    }

    printf("Display initialized successfully. Testing colors...\n");

    // 1. Red fill
    display_clear(COLOR_RED);
    display_present();
    sleep(1);

    // 2. Green fill
    display_clear(COLOR_GREEN);
    display_present();
    sleep(1);

    // 3. Blue fill
    display_clear(COLOR_BLUE);
    display_present();
    sleep(1);

    // 4. White canvas with a centered 10x10 black square
    display_clear(COLOR_WHITE);
    for (int y = 59; y < 69; ++y) {
        for (int x = 59; x < 69; ++x) {
            display_draw_pixel(x, y, COLOR_BLACK);
        }
    }
    display_present();
    sleep(2);

    display_close();
    printf("Test complete.\n");
    return 0;
}