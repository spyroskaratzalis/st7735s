#include <stdio.h>
#include <unistd.h>
#include "st7735s/display.h"
#include "st7735s/graphics.h"
#include "st7735s/image.h"

int main(void)
{
    // 1. Initialize ST7735S hardware
    if (display_init() != 0) {
        fprintf(stderr, "Failed to initialize display\n");
        return 1;
    }

    printf("Loading test.bmp...\n");

    // 2. Load the BMP using your custom parser
    Image *img = image_load_bmp("bigchungus.bmp");
    if (!img) {
        fprintf(stderr, "Failed to load test.bmp\n");
        display_close();
        return 1;
    }

    printf("Loaded image: %dx%d px\n", img->width, img->height);

    // 3. Clear the screen to black
    display_clear(COLOR_BLACK);

    // 4. Blit image centered at (0, 0)
    graphics_draw_image(0, 0, img);

    // 5. Push to physical screen
    display_present();
    printf("Image pushed to display. Showing for 5 seconds...\n");

    sleep(5);

    // 6. Clean up memory and hardware handles
    image_free(img);
    display_close();

    printf("Test complete and resources released.\n");
    return 0;
}