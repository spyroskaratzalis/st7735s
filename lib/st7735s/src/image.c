#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
            break;
        }

        for(int c = 0; c<width; c++) {
            uint8_t b = row_buf[c * 3 + 0];
            uint8_t g = row_buf[c * 3 + 1];
            uint8_t r = row_buf[c * 3 + 2];

            uint16_t color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
            image->pixels[target_row * width + c] = color;
        }

        if(padding > 0) {
            fseek(fp, padding, SEEK_CUR);
        }
    }

    free(row_buf);
    fclose(fp);
    return image;
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