#include "st7735s/image.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define STB_IMAGE_IMPLEMENTATION
#include <st7735s/stb_image.h>

Image* image_load_bmp(const char *filepath) {
    FILE* fp = fopen(filepath, "rb");
    if (fp == NULL) {
        perror("Error opening file");
        return NULL;
    }

    BMPFileHeader file_hdr;
    BMPInfoHeader info_hdr;

    if (fread(&file_hdr, sizeof(BMPFileHeader), 1, fp) != 1 ||
        fread(&info_hdr, sizeof(BMPInfoHeader), 1, fp) != 1) {
        fprintf(stderr, "image_load_bmp: Failed to read headers\n");
        fclose(fp);
        return NULL;
    }

    if(file_hdr.bfType != 0x4D42) {
        fprintf(stderr, "image_load_bmp: Not a BMP file (magic: 0x%04X)\n", file_hdr.bfType);
        fclose(fp);
        return NULL;
    }

    if(info_hdr.biBitCount != 24 || info_hdr.biCompression != 0) {
        fprintf(stderr, "image_load_bmp: Only uncompressed 24-bit BMPs supported\n");
        fclose(fp);
        return NULL;
    }
    int width = info_hdr.biWidth;
    bool bottom_up = (info_hdr.biHeight > 0);
    int height = abs(info_hdr.biHeight);

    if (width <= 0 || height <= 0) {
        fprintf(stderr, "image_load_bmp: Invalid dimensions %dx%d\n", width, height);
        fclose(fp);
        return NULL;
    }

    Image* image = malloc(sizeof(Image));
    if(!image) {
        fclose(fp);
        return NULL;
    }

    image->height = height;
    image->width = width;

    image->pixels = malloc(sizeof(uint16_t) * width * height);
    if(!image->pixels) {
        free(image);
        fclose(fp);
        return NULL;
    }

    fseek(fp, file_hdr.bfOffBits, SEEK_SET);
    int padding = (4 - ((width * 3) % 4)) % 4;
    uint8_t *row_buf = malloc(width * 3);
    if(!row_buf) {
        free(image->pixels);
        free(image);
        fclose(fp);
        return NULL;
    }

    for(int row = 0; row < height; row++) {
        int target_row = bottom_up ? (height - 1 - row) : row;

        if(fread(row_buf, 3, width, fp) != (size_t)width) {
            fprintf(stderr, "image_load_bmp: Unexpected end of file at row %d\n", row);
            free(row_buf);
            free(image->pixels);
            free(image);
            fclose(fp);
            return NULL;
        }

        for(int c = 0; c < width; c++) {
            uint8_t b = row_buf[c * 3 + 0];
            uint8_t g = row_buf[c * 3 + 1];
            uint8_t r = row_buf[c * 3 + 2];

            image->pixels[target_row * width + c] = rgb_to_rgb565(r, g, b);
        }

        if(padding > 0) {
            fseek(fp, padding, SEEK_CUR);
        }
    }

    free(row_buf);
    fclose(fp);
    return image;
}

Image* image_load_gif(const char *filepath, int *out_num_frames) {
    if (!filepath || !out_num_frames) return NULL;
    *out_num_frames = 0;

    FILE *f = fopen(filepath, "rb");
    if (!f) {
        fprintf(stderr, "Error: Could not open %s\n", filepath);
        return NULL;
    }

    // Read file into memory
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (file_size <= 0) {
        fclose(f);
        return NULL;
    }
    size_t size = (size_t)file_size;

    unsigned char *gif_data = (unsigned char *)malloc(size);
    if (!gif_data) {
        fclose(f);
        return NULL;
    }
    if (fread(gif_data, 1, size, f) != size) {
        fprintf(stderr, "Error: Failed to read complete GIF data from %s\n", filepath);
        free(gif_data);
        fclose(f);
        return NULL;
    }
    fclose(f);

    int width = 0, height = 0, num_frames = 0;
    int *delays = NULL;

    // Decode GIF buffer to raw RGBA
    unsigned char *raw_frames = stbi_load_gif_from_memory(
        gif_data, (int)size, &delays, &width, &height, &num_frames, NULL, 4
    );

    free(gif_data);
    free(delays); // Discard delay metadata

    if (!raw_frames || num_frames <= 0) {
        fprintf(stderr, "Error: Failed to decode GIF %s\n", filepath);
        return NULL;
    }

    // Allocate the Image struct array
    Image *image_array = (Image *)malloc(num_frames * sizeof(Image));
    if (!image_array) {
        stbi_image_free(raw_frames);
        return NULL;
    }

    int pixels_per_frame = width * height;
    int rgba_stride = pixels_per_frame * 4;

    // Convert each frame to RGB565
    for (int f = 0; f < num_frames; f++) {
        image_array[f].width = width;
        image_array[f].height = height;
        image_array[f].pixels = (uint16_t *)malloc(pixels_per_frame * sizeof(uint16_t));
        
        if (!image_array[f].pixels) {
            // Unwind allocations on failure
            for (int i = 0; i < f; i++) free(image_array[i].pixels);
            free(image_array);
            stbi_image_free(raw_frames);
            return NULL;
        }

        unsigned char *src_rgba = raw_frames + (f * rgba_stride);
        for (int p = 0; p < pixels_per_frame; p++) {
            uint8_t r = src_rgba[p * 4 + 0];
            uint8_t g = src_rgba[p * 4 + 1];
            uint8_t b = src_rgba[p * 4 + 2];
            image_array[f].pixels[p] = rgb_to_rgb565(r, g, b);
        }
    }

    stbi_image_free(raw_frames);
    *out_num_frames = num_frames;
    return image_array;
}

void image_array_free(Image *image_array, int num_frames) {
    if (!image_array) return;
    for (int f = 0; f < num_frames; f++) {
        free(image_array[f].pixels);
    }
    free(image_array);
}

void image_free(Image *img)
{
    if (img) {
        if (img->pixels) {
            free(img->pixels);
        }
        free(img);
    }
}