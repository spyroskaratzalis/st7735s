#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "input.h"

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

    printf("Input driver ready. Press buttons/joystick (Ctrl+C to quit)...\n");

    input_state state = {0};

    while (running) {
        input_update(&state);

        // 1. Edge triggers (fires once per press, no spamming)
        if (input_just_pressed(&state, KEY_1)) {
            printf("[EDGE] KEY1 clicked once\n");
        }
        if (input_just_pressed(&state, KEY_PRESS)) {
            printf("[EDGE] Joystick center pressed\n");
        }

        // 2. Chords / simultaneous checks (holding diagonal or multiple buttons)
        if (input_is_held(&state, KEY_UP | KEY_RIGHT)) {
            printf("[CHORD] Diagonal: UP + RIGHT held\n");
        }
        if (input_is_held(&state, KEY_1 | KEY_2)) {
            printf("[CHORD] KEY1 + KEY2 held together\n");
        }

        // 3. Release check
        if (input_just_released(&state, KEY_1)) {
            printf("[EDGE] KEY1 released\n");
        }

        usleep(20000); // 20 ms poll rate (~50 Hz)
    }

    input_close();
    printf("\nClean exit.\n");
    return 0;
}