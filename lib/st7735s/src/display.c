#include <lgpio.h>
#include <stdbool.h>
#include <unistd.h>
#include "st7735s/display.h"
#include <stdio.h>

#define SPI_BUS 0
#define DEVICE 0
#define SPI_BAUDRATE 15000000
#define SPI_FLAGS 0

#define DC_PIN 25
#define RST_PIN 27
#define BL_PIN 24

#define LCD_OFFSET_X 1
#define LCD_OFFSET_Y 2

#define CHUNK_PIXELS 1024

static int s_spi_handle = -1;
static int s_gpio_handle = -1;
static uint16_t s_framebuffer[LCD_WIDTH * LCD_HEIGHT];

typedef struct {
    uint8_t cmd;
    uint8_t data_len;
    uint8_t data[16];
    uint16_t delay_ms;
} DisplayInitCmd;

static const DisplayInitCmd INIT_SEQUENCE[] = {
    // 1. Software Reset / Sleep Out
    { 0x11, 0,  {0}, 120 },                                                // SLPOUT + 120ms wait

    // 2. Frame Rate Control
    { 0xB1, 3,  {0x01, 0x2C, 0x2D}, 0 },                                  // FRMCTR1 (Normal)
    { 0xB2, 3,  {0x01, 0x2C, 0x2D}, 0 },                                  // FRMCTR2 (Idle)
    { 0xB3, 6,  {0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D}, 0 },                // FRMCTR3 (Partial)

    // 3. Inversion Control
    { 0xB4, 1,  {0x07}, 0 },                                              // INVCTR (Dot inversion)

    // 4. Power Control Settings
    { 0xC0, 3,  {0xA2, 0x02, 0x84}, 0 },                                  // PWCTR1
    { 0xC1, 1,  {0xC5}, 0 },                                              // PWCTR2
    { 0xC2, 2,  {0x0A, 0x00}, 0 },                                        // PWCTR3
    { 0xC3, 2,  {0x8A, 0x2A}, 0 },                                        // PWCTR4
    { 0xC4, 2,  {0x8A, 0xEE}, 0 },                                        // PWCTR5
    { 0xC5, 1,  {0x0E}, 0 },                                              // VMCTR1 (VCOM)

    // 5. Memory Access Control (Orientation: BGR order, row/col exchange)
    { 0x36, 1,  {0xC8}, 0 },                                              // MADCTL

    // 6. Gamma Calibration (Positive & Negative curves)
    { 0xE0, 16, {0x0F, 0x1A, 0x0F, 0x18, 0x2F, 0x28, 0x20, 0x22,
                 0x1F, 0x1B, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10}, 0 },    // GMCTRP1
    { 0xE1, 16, {0x0F, 0x1B, 0x0F, 0x17, 0x33, 0x2C, 0x29, 0x2E,
                 0x30, 0x30, 0x39, 0x3F, 0x00, 0x07, 0x03, 0x10}, 0 },    // GMCTRN1

    // 7. Color Format: 16-bit RGB565
    { 0x3A, 1,  {0x05}, 0 },                                              // COLMOD

    // 8. Turn On Display
    { 0x29, 0,  {0}, 100 },                                               // DISPON + 100ms wait
};

#define INIT_CMD_COUNT (sizeof(INIT_SEQUENCE) / sizeof(INIT_SEQUENCE[0]))

static void display_write_cmd(uint8_t cmd) {
    lgGpioWrite(s_gpio_handle, DC_PIN, 0);
    lgSpiWrite(s_spi_handle, (const char*)&cmd, 1);
}

static void display_write_data_byte(uint8_t data) {
    lgGpioWrite(s_gpio_handle, DC_PIN, 1);
    lgSpiWrite(s_spi_handle, (const char*)&data, 1);
}

static void display_hw_reset() {
    lgGpioWrite(s_gpio_handle, RST_PIN, 1);
    usleep(10000);
    lgGpioWrite(s_gpio_handle, RST_PIN, 0);
    usleep(20000);
    lgGpioWrite(s_gpio_handle, RST_PIN, 1);
    usleep(20000);
}

static void display_set_window(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
    x0 += LCD_OFFSET_X;
    x1 += LCD_OFFSET_X;
    y0 += LCD_OFFSET_Y;
    y1 += LCD_OFFSET_Y;

    display_write_cmd(0x2a);
    display_write_data_byte(0x00);
    display_write_data_byte(x0);
    display_write_data_byte(0x00);
    display_write_data_byte(x1);

    display_write_cmd(0x2b);
    display_write_data_byte(0x00);
    display_write_data_byte(y0);
    display_write_data_byte(0x00);
    display_write_data_byte(y1);
}

int display_init() {
    if(s_spi_handle >= 0 && s_gpio_handle >= 0) return 0; // Already initialized

    s_gpio_handle = lgGpiochipOpen(DEVICE);
    if(s_gpio_handle < 0) {
        perror("st7735s/display: Failed to open gpiochip 0");
        return -1;
    }

    if (lgGpioClaimOutput(s_gpio_handle, 0, DC_PIN, 0) < 0 ||
        lgGpioClaimOutput(s_gpio_handle, 0, RST_PIN, 0) < 0 ||
        lgGpioClaimOutput(s_gpio_handle, 0, BL_PIN, 0) < 0) {
            fprintf(stderr, "st7735s/display: Failed to claim GPIO outputs\n");
            lgGpiochipClose(s_gpio_handle);
            s_gpio_handle = -1;
            return -1;
    }

    s_spi_handle = lgSpiOpen(SPI_BUS, DEVICE, SPI_BAUDRATE, SPI_FLAGS);
    if (s_spi_handle < 0) {
        fprintf(stderr, "st7735s/display: Failed to open SPI (error %d)\n",
                    s_spi_handle);
        lgGpiochipClose(s_gpio_handle);
        s_spi_handle = -1;
        return -1;
    }

    display_hw_reset();

    for (size_t i = 0; i < INIT_CMD_COUNT; ++i) {
            const DisplayInitCmd *entry = &INIT_SEQUENCE[i];
            display_write_cmd(entry->cmd);
            for (uint8_t d = 0; d < entry->data_len; ++d) {
                display_write_data_byte(entry->data[d]);
            }
            if (entry->delay_ms > 0) {
                usleep(entry->delay_ms * 1000);
            }
    }

    lgGpioWrite(s_gpio_handle, BL_PIN, 1);

    display_clear(COLOR_BLACK);
    return 0;
}

void display_close() {
    if(s_gpio_handle >= 0) {
        lgGpioWrite(s_gpio_handle, BL_PIN, 0);
        lgGpiochipClose(s_gpio_handle);
        s_gpio_handle = -1;
    }
    if(s_spi_handle >= 0) {
        lgSpiClose(s_spi_handle);
        s_spi_handle = -1;
    }
}

void display_clear(uint16_t color) {
    for( int i = 0; i<LCD_WIDTH * LCD_HEIGHT; i++) {
        s_framebuffer[i] = color;
    }
}

void display_draw_pixel(int x, int y, uint16_t color) {
    if(x < 0 || x >= LCD_WIDTH || y < 0 || y >= LCD_HEIGHT) return;
    s_framebuffer[y * LCD_WIDTH + x] = color;
}

void display_present() {
    if (s_gpio_handle < 0 || s_spi_handle < 0) return;

    display_set_window(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);
    display_write_cmd(0x2c);

    lgGpioWrite(s_gpio_handle, DC_PIN, 1);
    uint8_t tx_buf[CHUNK_PIXELS * 2];
    size_t total_pixels = LCD_WIDTH * LCD_HEIGHT;
    size_t offset = 0;

    while(offset < total_pixels) {
        size_t pixels_to_send = total_pixels - offset;
        if(pixels_to_send > CHUNK_PIXELS) {
            pixels_to_send = CHUNK_PIXELS;
        }
        for(size_t i = 0; i < pixels_to_send; i++) {
            uint16_t pixel = s_framebuffer[offset + i];
            tx_buf[2*i] = (uint8_t)(pixel>>8);
            tx_buf[2*i + 1] = (uint8_t)(pixel & 0xFF);
        }

        lgSpiWrite(s_spi_handle, (const char*)tx_buf, (int)(pixels_to_send*2));
        offset += pixels_to_send;
    }
}

void display_set_backlight(bool enable) {
    if(s_gpio_handle >= 0)
        lgGpioWrite(s_gpio_handle, BL_PIN, enable ? 1 : 0);
}