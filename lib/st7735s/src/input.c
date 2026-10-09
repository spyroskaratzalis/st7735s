#include "st7735s/input.h"
#include <stdio.h>
#include <stddef.h>
#include <lgpio.h>

typedef struct {
    int pin;
    KeyMask mask;
} KeyMapping;

static const KeyMapping KEY_MAP[] = {
    { 6,  KEY_UP    },
    { 19, KEY_DOWN  },
    { 5,  KEY_LEFT  },
    { 26, KEY_RIGHT },
    { 13, KEY_PRESS },
    { 21, KEY_1     },
    { 20, KEY_2     },
    { 16, KEY_3     },
};

#define KEY_COUNT (sizeof(KEY_MAP) / sizeof(KEY_MAP[0]))

static int s_gpio_handle = -1;
static uint8_t s_prev_raw = 0;

int input_init() {
    if(s_gpio_handle >= 0) return 0; // Already Initialized
    s_gpio_handle = lgGpiochipOpen(0);
    if(s_gpio_handle < 0)  return -1; // Failed to open
    for(size_t i = 0; i < KEY_COUNT; i++) {
        int status = lgGpioClaimInput(s_gpio_handle, LG_SET_PULL_UP, KEY_MAP[i].pin);
        if(status < 0) {
            fprintf(stderr, "st7735s/input: Failed to claim GPIO %d with pull-up (error %d)\n",
                    KEY_MAP[i].pin, status);
            lgGpiochipClose(s_gpio_handle);
            s_gpio_handle = -1;
            return -1;
        }
    }
    s_prev_raw = 0;
    return 0;
}

void input_update(input_state *state) {
    if(!state) return;
    if(s_gpio_handle < 0) {
        state->current_state = 0;
        state->pressed_edge  = 0;
        state->released_edge = 0;
        return;
    }

    uint8_t raw = 0;
    for(size_t i = 0; i<KEY_COUNT; i++) {
        int level = lgGpioRead(s_gpio_handle, KEY_MAP[i].pin);
        if(level == 0) raw |= KEY_MAP[i].mask;
    }

    state->current_state = raw;
    state->pressed_edge = raw & (uint8_t)(~s_prev_raw);
    state->released_edge = (uint8_t)(~raw) & s_prev_raw;

    s_prev_raw=raw;
}

void input_close(void) {
    if(s_gpio_handle >= 0) {
        lgGpiochipClose(s_gpio_handle);
        s_gpio_handle = -1;
    }
    s_prev_raw = 0;
}
