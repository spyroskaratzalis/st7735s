#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "st7735s/input.h"

static volatile bool running = true;

static void handle_sigint(int sig) {
    (void)sig;
    running = false;
}

int main(void) {
    signal(SIGINT, handle_sigint);

    if (input_init() != 0) {
        fprintf(stderr, "Failed to initialize inputs\n");
        return 1;
    }

    printf("==========================================\n");
    printf(" ST7735S Input Test - Press Any Button\n");
    printf(" (Press Ctrl+C to exit)\n");
    printf("==========================================\n");

    input_state state = {0};

    while (running) {
        input_update(&state);

        // Joystick directions
        if (input_just_pressed(&state, KEY_UP))    printf("-> JOYSTICK UP\n");
        if (input_just_pressed(&state, KEY_DOWN))  printf("-> JOYSTICK DOWN\n");
        if (input_just_pressed(&state, KEY_LEFT))  printf("-> JOYSTICK LEFT\n");
        if (input_just_pressed(&state, KEY_RIGHT)) printf("-> JOYSTICK RIGHT\n");
        if (input_just_pressed(&state, KEY_PRESS)) printf("-> JOYSTICK PRESS\n");

        // Action buttons
        if (input_just_pressed(&state, KEY_1))     printf("-> KEY 1\n");
        if (input_just_pressed(&state, KEY_2))     printf("-> KEY 2\n");
        if (input_just_pressed(&state, KEY_3))     printf("-> KEY 3\n");

        // Release events
        if (input_just_released(&state, KEY_1))    printf("   (KEY 1 released)\n");
        if (input_just_released(&state, KEY_PRESS))printf("   (JOYSTICK released)\n");

        usleep(20000); // 20ms poll rate (50Hz)
    }

    input_close();
    printf("\nClean shutdown.\n");
    return 0;
}