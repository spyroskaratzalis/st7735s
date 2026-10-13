#include "st7735s/st7735s_hal.h"

typedef struct {
    int gpio_handle;
    int spi_handle;
    int dc_gpio_num;
    int rst_gpio_num;
} lgpio_ctx_t;

static void lgpio_gpio_write(void *context, )