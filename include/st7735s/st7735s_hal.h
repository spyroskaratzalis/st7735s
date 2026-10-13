#include <stdint.h>
#include <stdlib.h>

typedef enum {
    ST7735S_PIN_DC,
    ST7735S_PIN_RST
} st7735s_pin_t;


typedef struct {
    void *context;
    void (*gpio_write)(void* context, st7735s_pin_t pin, int level);
    void (*spi_write)(void* context, st7735s_pin_t pin, const uint8_t data, size_t length);
    void (*delay_ms)(uint8_t millisecond);
} st7735s_hal_t;