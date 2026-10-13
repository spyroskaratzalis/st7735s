#ifndef ST7735S_IMAGE_H
#define ST7735S_IMAGE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Standard 14-byte Windows BMP file header.
 * Packed with __attribute__((packed)) to guarantee field alignment matching disk layout.
 */
typedef struct {
    uint16_t bfType;          /** Magic bytes: must be 0x4D42 ('BM') */
    uint32_t bfSize;          /** Total file size in bytes */
    uint16_t bfReserved1;     /** Reserved, must be 0 */
    uint16_t bfReserved2;     /** Reserved, must be 0 */
    uint32_t bfOffBits;       /** Offset from start of file to actual raster pixel data */
} __attribute__((packed)) BMPFileHeader;

/**
 * Standard 40-byte Windows DIB info header (BITMAPINFOHEADER).
 */
typedef struct {
    uint32_t biSize;          /** Header structure size (must be 40) */
    int32_t  biWidth;         /** Image width in pixels */
    int32_t  biHeight;        /** Height in pixels (positive = bottom-up, negative = top-down) */
    uint16_t biPlanes;        /** Number of color planes (must be 1) */
    uint16_t biBitCount;      /** Bits per pixel (this driver supports 24-bit uncompressed) */
    uint32_t biCompression;   /** Compression type: 0 = BI_RGB (uncompressed) */
    uint32_t biSizeImage;     /** Image data size in bytes (may be 0 for uncompressed) */
    int32_t  biXPelsPerMeter; /** Target horizontal resolution */
    int32_t  biYPelsPerMeter; /** Target vertical resolution */
    uint32_t biClrused;       /** Number of indexed palette colors (0 = 2^biBitCount) */
    uint32_t biClrImportant;  /** Number of required palette colors (0 = all) */
} __attribute__((packed)) BMPInfoHeader;

/**
 * In-memory representation of an uncompressed graphic.
 * Pixels are stored as a contiguous 1D array of 16-bit RGB565 words
 * arranged in top-down row-major order: index = (y * width) + x.
 */
typedef struct {
    int width;                /** Image width in pixels */
    int height;               /** Image height in pixels */
    uint16_t* pixels;         /** Contiguous heap buffer of (width * height) RGB565 words */
} Image;

/**
 * Converts 24-bit RGB (8 bits per channel) into a 16-bit packed RGB565 value.
 *
 * Channel distribution:
 *   Bits 15..11: Red   (5 bits)
 *   Bits 10..5:  Green (6 bits)
 *   Bits 4..0:   Blue  (5 bits)
 *
 * @param r Red channel value (0-255).
 * @param g Green channel value (0-255).
 * @param b Blue channel value (0-255).
 * @return Packed 16-bit RGB565 value.
 */
static inline uint16_t rgb_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

/**
 * Backward-compatible alias for rgb_to_rgb565.
 */
static inline uint16_t rgba_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return rgb_to_rgb565(r, g, b);
}

/**
 * Loads an uncompressed 24-bit Windows BMP image from disk into an Image struct.
 *
 * Handles:
 * - BMP header validation (magic 0x4D42, 24-bit depth, BI_RGB uncompressed).
 * - Automatic conversion from bottom-up scanlines to top-down row order.
 * - Scanline row padding (aligns each row to 4-byte boundaries).
 *
 * Memory ownership:
 * Caller owns the returned pointer and must free it via image_free().
 *
 * @param filepath Path to .bmp file.
 * @return Allocated Image pointer on success, or NULL if file open or parse failed.
 */
Image* image_load_bmp(const char* filepath);

/**
 * Decodes all frames of an animated GIF from disk into an array of Image structs.
 *
 * Memory ownership:
 * Caller owns the returned array and must release it with image_array_free().
 *
 * @param filepath Path to .gif file.
 * @param[out] out_num_frames Receives the total number of frames decoded.
 * @return Dynamically allocated array of Image structures, or NULL on failure.
 */
Image* image_load_gif(const char* filepath, int* out_num_frames);

/**
 * Destroys a single Image previously returned by image_load_bmp().
 * Frees both the pixel buffer and the Image struct. Safe on NULL.
 *
 * @param img Pointer to Image.
 */
void image_free(Image* img);

/**
 * Destroys an array of Image structures previously returned by image_load_gif().
 * Frees each frame's pixel buffer, then the array buffer. Safe on NULL.
 *
 * @param image_array Pointer to Image array.
 * @param num_frames Number of frames in array.
 */
void image_array_free(Image* image_array, int num_frames);

#ifdef __cplusplus
}
#endif

#endif // ST7735S_IMAGE_H