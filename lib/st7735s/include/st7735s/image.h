#ifndef ST7735S_IMAGE_H
#define ST7735S_IMAGE_H

#include <stdint.h>

typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReversed2;
    uint32_t bfOffBits;
} __attribute__((packed)) BMPFileHeader;

typedef struct {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrused;
    uint32_t biClrImportant;
} __attribute__((packed)) BMPInfoHeader;


typedef struct {
    int width;
    int height;
    uint16_t *pixels; // Allocated RGB565 buffer
} Image;

Image* image_load_bmp(const char *filepath);
void   image_free(Image *img);

#endif