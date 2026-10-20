# ST7735S C Driver Library

A lightweight, memory-buffered C11 driver I built for the Sitronix **ST7735S** TFT LCD controller, specifically targeting the **Waveshare 1.44" 128x128 SPI LCD HAT** on Raspberry Pi running modern Linux kernels.

---

## 1. Why I Built This

The Wavshare 1.44" LCD HAT is very painful to use:

- **Dead libraries:** It relied heavily on `wiringPi` (abandoned years ago) and direct physical register access via `bcm2835` (which flat-out crashes on modern 6.x kernels and the Raspberry Pi 5).
- **Spaghetti architecture:** Hundreds of lines of register pokes, fonts, button polling, and BMP decoding crammed into monolithic files with messy global state.
- **Painfully slow rendering:** Primitives were drawn directly over SPI one pixel or line at a time. Every line required multiple SPI command handshakes, resulting in visible tearing, sluggish frame rates, and massive syscall overhead.

---

## 2. This Isn't a "Modern" Driver (Yet)

I want to be upfront: **this is not an industry-standard, production-grade embedded driver.** 

While it's a massive upgrade over the original driver, it's far from modern or complete. Here are a few issues I plan on fixing:

1. **It relies on global static singletons:** If you look at `display.c`, handles and buffers (`s_spi_handle`, `s_framebuffer`, `s_gpio_handle`) are file-scoped `static` variables. If you wanted to run two screens side-by-side on CE0 and CE1, you couldn't do it without duplicating or rewriting the code.
2. **It's tightly coupled to Linux and `lgpio`:** Real embedded drivers don't `#include <lgpio.h>`. They don't know what an OS is. They communicate through a decoupled Hardware Abstraction Layer (HAL) or bus interface. This driver won't compile on an STM32, ESP32, or desktop simulator without surgery.
3. **It flushes the entire screen every frame:** `display_present()` dumps all 16,384 pixels (32 KiB) across SPI regardless of whether you changed one pixel or the whole screen. It lacks dirty-rectangle tracking.
4. **It uses synchronous, blocking SPI:** The CPU waits around while the kernel transmits chunks over SPI, rather than firing off asynchronous DMA transfers in the background.
5. **It hardcodes panel dimensions and offsets:** The 128x128 resolution and `(1, 2)` glass offsets are baked into `#define` constants rather than dynamic runtime descriptors.
6. **It bundles buttons with the display:** An ST7735S chip has no idea what a joystick is. In a proper architecture, inputs live in a Board Support Package (BSP) or input subsystem, completely separate from the display driver.

Here is an breakdown of my code compared to industrial embedded standards:

| Dimension | Current Version | Industry-Standard Production C |
| :--- | :--- | :--- |
| **Instance Model** | Global singletons (`static uint16_t s_framebuffer[]`) | **Opaque context handles** (`st7735s_t* dev`) passed to every API function. |
| **Hardware Coupling** | Direct calls to Linux `lgpio` and `/dev/spidev0.0` | **Abstracted Bus / HAL Layer** via function pointers (`write_cmd`, `write_data`, `set_pin`). |
| **RAM Footprint** | Fixed 32 KiB static framebuffer | **Configurable buffering modes**: full buffer, dual ping-pong line buffers (1–2 KiB RAM for microcontrollers), or direct streaming. |
| **Dirty Tracking** | Dumps the full 32 KiB on every present | **Bounding-box dirty rect tracking**: only stream the bounding box of modified pixels. |
| **SPI Transfers** | Synchronous blocking chunked writes | **Non-blocking DMA transfers**: SPI hardware pumps pixels while the CPU prepares the next frame. |
| **Panel Support** | Hardcoded constants for 1.44" 128x128 | **Panel profiles**: runtime descriptors supporting 128x160, 80x160, and offset variants dynamically. |
| **Input System** | Joystick and buttons baked into `input.c` | **Decoupled BSP**: input handled independently via kernel `evdev` or dedicated input drivers. |

---

## 3. Why I Made The Choices I Did

### A. Choosing `lgpio` over `wiringPi` or `/dev/mem`
Older Raspberry Pi tutorials tell you to use `wiringPi` or `bcm2835`. On modern kernels (6.6+) and the Raspberry Pi 5, those fail because direct register access via `/dev/mem` is blocked and GPIO registers have physically moved to the RP1 southbridge chip. 

I picked **`lgpio`** because it uses the modern Linux GPIO character device interface (`/dev/gpiochip*`). This gives me kernel-level pin arbitration, clean process cleanup on `Ctrl+C`, and internal pull-up support (`LG_SET_PULL_UP`) out of the box.

### B. Choosing a 32 KiB In-Memory Framebuffer
Drawing directly over SPI requires sending an 11-byte window command sequence (`CASET`, `RASET`, `RAMWR`) for every primitive you draw. If you draw a 100-pixel line, that’s dozens of SPI bus transactions and context switches.

Instead, I allocated a contiguous **32 KiB buffer** in RAM (`128 * 128 * 2` bytes for RGB565):
- All drawing functions (`graphics_draw_line`, `graphics_draw_image`, etc.) write directly to RAM via simple pointer arithmetic.
- Boundary clipping is virtually free in software (negative or out-of-bounds coordinates are safely dropped).
- Pixels are only transferred to the screen when calling `display_present()`, giving me **tear-free, atomic frame updates**.

### C. 2048-Byte SPI Chunking
Why not send all 32,768 bytes in a single `lgSpiWrite()` call?

The Linux kernel's `spidev` driver defaults to a maximum buffer size of **4096 bytes** (`bufsiz`). Calling `write()` with 32 KiB in one shot causes `EMSGSIZE` or silent truncation depending on your kernel configuration.
By chunking transfers into **1024 pixels (2048 bytes)**:
- I stay comfortably within the default 4096-byte kernel limit.
- I minimize syscall overhead (only 16 writes to flush an entire frame).
- At 15 MHz SPI, flushing the entire display takes roughly 17–20 ms (~50–60 theoretical FPS).

### D. Active-Low Inputs & Hardware Pull-Ups
The Waveshare HAT wires all 5 joystick switches and 3 push buttons to ground when pressed (active-low). 

Without pull-up resistors, disconnected inputs pick up electrical noise and float. In `input_init()`, I claim all 8 lines with `LG_SET_PULL_UP`. The driver inverts the reading internally so application code can treat a `1` as "pressed" without dealing with active-low hardware quirks.

### E. Deterministic, Self-Contained Edge Detection
Instead of spawning a background polling thread or using messy timer callbacks, I compute edge transitions directly in `input_update()`:
```c
uint8_t prev = state->current_state;
state->current_state = raw;
state->pressed_edge  = raw & (uint8_t)(~prev);
state->released_edge = (uint8_t)(~raw) & prev;
```
This keeps `input_state` self-contained, reentrant, and guarantees that `input_just_pressed()` fires for exactly one frame per press.

### F. Handling the 132x162 Silicon Offset
The ST7735S silicon has enough memory for 132 columns by 162 rows. But the Waveshare 1.44" board uses a 128x128 glass panel centered inside that memory grid.

Depending on the panel's scan orientation (`MADCTL` register `0x36`), the visible display starts at an offset:
- `LCD_OFFSET_X = 1`
- `LCD_OFFSET_Y = 2`

These offsets are built into `display_set_window()` so the drawing code can work in an intuitive `(0, 0)` to `(127, 127)` coordinate space.

---

## 4. Codebase Layout

```
─ include/st7735s/
  ├── st7735s.h     # Master umbrella header
  ├── display.h     # Hardware lifecycle, SPI management, and framebuffer flushing
  ├── graphics.h    # 2D primitives: lines, rects, circles, bitmaps, clipping
  ├── image.h       # 24-bit uncompressed BMP parser and animated GIF decoder
  ├── input.h       # 5-way joystick & 3 user button inputs with edge triggers
  └── stb_image.h   # Public-domain image decoder engine
─ src/
    ├── display.c     # Display initialization, reset timing, SPI streaming
    ├── graphics.c    # Raster algorithms (Bresenham lines, scanline circles)
    ├── image.c       # BMP and GIF loading pipelines
    └── input.c       # lgpio sampling and edge calculations
```

---

## 5. Quick Usage Example

```c
#include <stdio.h>
#include <st7735s/st7735s.h>

int main(void) {
    // 1. Initialize hardware
    if (display_init() != 0 || input_init() != 0) {
        fprintf(stderr, "Failed to initialize hardware\n");
        return 1;
    }

    input_state inputs = {0};

    // 2. Clear framebuffer to dark gray
    display_clear(COLOR_GRAY);

    // 3. Draw some vector shapes
    graphics_draw_line(0, 0, 127, 127, COLOR_RED);
    graphics_fill_rect(20, 20, 40, 30, COLOR_BLUE);
    graphics_fill_circle(90, 90, 20, COLOR_GREEN);
    graphics_draw_circle(90, 90, 24, COLOR_YELLOW);

    // 4. Push drawing to the physical screen
    display_present();

    // 5. Interactive loop
    while (!input_just_pressed(&inputs, KEY_3)) {
        input_update(&inputs);
        if (input_is_held(&inputs, KEY_UP)) {
            // Handle joystick UP...
        }
    }

    // 6. Cleanup
    input_close();
    display_close();
    return 0;
}
```

---

## 6. How to Build & Run

### Dependencies
Install `lgpio` on your Raspberry Pi:
```bash
sudo apt-get update
sudo apt-get install -y liblgpio-dev cmake gcc
```

### Enable SPI
Ensure hardware SPI is enabled in raspi-config:
```bash
sudo raspi-config
# Interface Options -> SPI -> Yes
```

### Build
```bash
mkdir build && cd build
cmake ..
cmake --build .
./bin/lcd_c
```

---

## 7. What I Plan to Improve Next

When I refactor this towards an industrial-grade driver, my roadmap will be:
- [ ] **Decouple the HAL:** Abstract SPI and GPIO into a `st7735s_bus_t` interface so this library can compile on microcontrollers or an SDL desktop simulator.
- [ ] **Eliminate Global State:** Wrap the display into a heap-allocated or user-allocated `st7735s_t` instance struct.
- [ ] **Add Dirty Rectangles:** Track `min_x`, `max_x`, `min_y`, `max_y` to only stream modified pixels over SPI.
- [ ] **Separate the Input Layer:** Move the joystick and button code out of the LCD driver and into a standalone BSP or Linux `evdev` handler.

---



## NOTE:
Big part of README and code comments helped by AI. Core logic written by myself, but using AI as general mentor for what I should improve.

## License & Credits

- Code released under the **MIT License**.
- Includes `stb_image.h` by Sean Barrett (Public Domain / MIT).
