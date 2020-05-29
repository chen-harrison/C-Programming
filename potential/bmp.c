#include "bmp.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

// calculate the number of bytes of memory needed to serialize the bitmap
// that is, to write a valid bmp file to memory
size_t bmp_calculate_size(bitmap_t *bmp) {
    size_t size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) +
                  bmp->width * bmp->height * sizeof(color_bgr_t);
    return size;
}

// write the bmp file to memory at data, which must be at least
// bmp_calculate_size large.
void bmp_serialize(bitmap_t *bmp, uint8_t *data) {
    BITMAPFILEHEADER file_header = {0};
    file_header.bfType = 0x4d42;
    file_header.bfSize = bmp_calculate_size(bmp);
    file_header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    BITMAPINFOHEADER info_header = {0};
    info_header.biSize = 0x00000028;
    info_header.biWidth = bmp->width;
    info_header.biHeight = bmp->height;
    info_header.biPlanes = 1;
    info_header.biBitCount = 24;
    info_header.biXPelsPerMeter = 2835;
    info_header.biYPelsPerMeter = 2835;
    uint8_t *data_out = data;
    memcpy(data_out, &file_header, sizeof(BITMAPFILEHEADER));
    data_out += sizeof(file_header);
    memcpy(data_out, &info_header, sizeof(BITMAPINFOHEADER));
    data_out += sizeof(info_header);

    int row_size = (int)(bmp->width * sizeof(color_bgr_t));
    for (int i = bmp->height; i > 0; i--) {
        int row_start = bmp->width * (i - 1);
        memcpy(data_out, &bmp->data[row_start], row_size);
        data_out += row_size;
    }
}
