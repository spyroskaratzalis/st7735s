#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "st7735s/display.h"
#include "st7735s/graphics.h"
#include "st7735s/image.h"
#include "st7735s/input.h"

#define FILEPATH "frames/spiderman.gif"

Image* frames = NULL;
int frame_count = 0;
volatile sig_atomic_t have_to_close = 0;

void intHandler(int sig) {
    have_to_close = 1;
}

int main(void) {
    bool paused = 0;
    input_state state = {0, 0, 0};

    signal(SIGINT, intHandler);

    if (display_init() != 0) {
        fprintf(stderr, "Failed to initialize display\n");
        return 1;
    }
    if (input_init() != 0) {
        fprintf(stderr, "Failed to initialize inputs\n");
        return 1;
    }
    display_clear(COLOR_BLACK);

    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    printf("Loading gif into ram.\n");

    frames = image_load_gif(FILEPATH, &frame_count);
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    if (!frames || frame_count <= 0) {
        fprintf(stderr, "Failed to load GIF frames from %s\n", FILEPATH);
        input_close();
        display_close();
        return 1;
    }

    double dt = (end_time.tv_sec - start_time.tv_sec) * 1000.0 +
                (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0;
    double fps = (double)frame_count * 1000.0 / dt;
    printf("Loaded %d frames into ram in %.2f milliseconds. That's %.2f frames per second\n", frame_count, dt, fps);

    display_clear(COLOR_BLACK);

    clock_gettime(CLOCK_MONOTONIC, &start_time);
    printf("Playing gif.");
    int i = 0;
    while (!have_to_close) {
        input_update(&state);
        if (input_just_pressed(&state, KEY_1)) {
            paused = !paused;
        }
        if (!paused) {
            graphics_draw_image(0, 0, frames + i);
            display_present();
            i = (i + 1) % frame_count;
        }        
    }
    image_array_free(frames, frame_count);
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    dt = (end_time.tv_sec - start_time.tv_sec) * 1000.0 +
         (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0;
    fps = (double)frame_count * 1000.0 / dt;
    printf("Played %d frames in %.2f milliseconds. That's %.2f frames per second\n", frame_count, dt, fps);

    input_close();
    display_close();

    printf("Test complete and resources released.\n");
    return 0;
}