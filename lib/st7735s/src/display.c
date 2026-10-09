#include <lgpio.h>
#include <stdio.h>

static int spi_handle = -1;

#define SPI_BUS 0
#define DEVICE 0
#define SPI_BAUDRATE 15000000
#define SPI_FLAGS 0

#define DC_PIN 25
#define RST_PIN 27
#define BL_PIN 24

int s_spi_handle = -1;
int s_gpio_handle = -1;

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
        fprintf(stderr, "st7735s/input: Failed to OPEN SPI (error %d)\n",
                    s_spi_handle);
        lgGpiochipClose(s_gpio_handle);
        s_spi_handle = -1;
        return -1;
    }



}