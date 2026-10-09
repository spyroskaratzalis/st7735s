#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    KEY_UP    = 1 << 0,
    KEY_DOWN  = 1 << 1,
    KEY_LEFT  = 1 << 2,
    KEY_RIGHT = 1 << 3,
    KEY_PRESS = 1 << 4,
    KEY_1     = 1 << 5,
    KEY_2     = 1 << 6,
    KEY_3     = 1 << 7,
} KeyMask;

typedef struct {
    uint8_t current_state;
    uint8_t pressed_edge;
    uint8_t released_edge;
} input_state;

int input_init(void);
void input_close(void);
void input_update(input_state *state);
static inline bool input_is_held(const input_state *state, KeyMask key) {
    return state && ((state->current_state & (uint8_t)key) == (uint8_t)key);
}
static inline bool input_just_pressed(const input_state *state, KeyMask key) {
    return state && ((state->pressed_edge & (uint8_t)key) == (uint8_t)key);
}
static inline bool input_just_released(const input_state *state, KeyMask key)
{
    return state && ((state->released_edge & (uint8_t)key) == (uint8_t)key);
}

#ifdef __cplusplus
}
#endif
#endif