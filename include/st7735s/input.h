#ifndef ST7735S_INPUT_H
#define ST7735S_INPUT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Button and Joystick bitmasks for the Waveshare 1.44" LCD HAT.
 *
 * Hardware design note:
 * All buttons and the 5-way joystick are wired with active-low logic:
 * - Line idling: pulled HIGH (3.3V) via internal pull-up resistors.
 * - Pressed: pulled LOW to Ground (0V).
 * The driver normalizes this so that a bit is set to 1 when a key is ACTIVE (pressed).
 */
typedef enum {
    KEY_UP    = 1 << 0, /** Joystick UP (BCM GPIO 6) */
    KEY_DOWN  = 1 << 1, /** Joystick DOWN (BCM GPIO 19) */
    KEY_LEFT  = 1 << 2, /** Joystick LEFT (BCM GPIO 5) */
    KEY_RIGHT = 1 << 3, /** Joystick RIGHT (BCM GPIO 26) */
    KEY_PRESS = 1 << 4, /** Joystick Center Press (BCM GPIO 13) */
    KEY_1     = 1 << 5, /** Key 1 push button (BCM GPIO 21) */
    KEY_2     = 1 << 6, /** Key 2 push button (BCM GPIO 20) */
    KEY_3     = 1 << 7, /** Key 3 push button (BCM GPIO 16) */
} KeyMask;

/**
 * Input state container tracking continuous press and single-frame edge transitions.
 *
 * Usage pattern:
 * Allocate one instance in your application (e.g. `input_state state = {0};`),
 * then call `input_update(&state)` once at the beginning of each frame.
 */
typedef struct {
    uint8_t current_state; /** Bitmask of keys currently held down */
    uint8_t pressed_edge;  /** Bitmask of keys that transitioned from released to pressed this frame */
    uint8_t released_edge; /** Bitmask of keys that transitioned from pressed to released this frame */
} input_state;

/**
 * Configures the GPIO inputs and internal pull-up resistors for the joystick and buttons.
 *
 * Claims GPIO lines 6, 19, 5, 26, 13, 21, 20, 16 on gpiochip 0 with LG_SET_PULL_UP.
 *
 * @return 0 on success, or -1 if gpiochip 0 failed to open or lines could not be claimed.
 */
int input_init(void);

/**
 * Releases GPIO line claims and closes the input GPIO chip handle.
 */
void input_close(void);

/**
 * Samples the physical hardware lines and updates edge detection inside the provided state.
 *
 * Should be called once per main loop iteration / frame tick.
 * Computes:
 *   pressed_edge  = current & ~previous
 *   released_edge = ~current & previous
 *
 * Reentrancy:
 * This function is fully self-contained using the passed state pointer.
 *
 * @param state Pointer to the input_state structure to update.
 */
void input_update(input_state *state);

/**
 * Checks if a specific key is currently held down.
 *
 * @param state Pointer to current input_state.
 * @param key KeyMask bit to test (e.g. KEY_UP, KEY_1).
 * @return true if the key is held, false otherwise.
 */
static inline bool input_is_held(const input_state *state, KeyMask key) {
    return state && ((state->current_state & (uint8_t)key) == (uint8_t)key);
}

/**
 * Checks if a specific key was pressed down during the current frame (single-shot pulse).
 *
 * @param state Pointer to current input_state.
 * @param key KeyMask bit to test.
 * @return true if the key transitioned to pressed this frame, false otherwise.
 */
static inline bool input_just_pressed(const input_state *state, KeyMask key) {
    return state && ((state->pressed_edge & (uint8_t)key) == (uint8_t)key);
}

/**
 * Checks if a specific key was released during the current frame.
 *
 * @param state Pointer to current input_state.
 * @param key KeyMask bit to test.
 * @return true if the key transitioned to released this frame, false otherwise.
 */
static inline bool input_just_released(const input_state *state, KeyMask key) {
    return state && ((state->released_edge & (uint8_t)key) == (uint8_t)key);
}

#ifdef __cplusplus
}
#endif

#endif // ST7735S_INPUT_H