#include "bmp.h"

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
    BITMAPFILEHEADER fileHeader = {0};
    fileHeader.bfType = 0x4d42;
    fileHeader.bfSize = bmp_calculate_size(bmp);
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    BITMAPINFOHEADER infoHeader = {0};
    infoHeader.biSize = 0x00000028;
    infoHeader.biWidth = bmp->width;
    infoHeader.biHeight = bmp->height;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = 24;
    infoHeader.biXPelsPerMeter = 2835;
    infoHeader.biYPelsPerMeter = 2835;
    uint8_t *dataOut = data;
    memcpy(dataOut, &fileHeader, sizeof(BITMAPFILEHEADER));
    dataOut += sizeof(fileHeader);
    memcpy(dataOut, &infoHeader, sizeof(BITMAPINFOHEADER));
    dataOut += sizeof(infoHeader);

    int rowSize = bmp->width * (int)sizeof(color_bgr_t);
    for (int i = bmp->height; i > 0; i--) {
        int rowStart = bmp->width * (i - 1);
        memcpy(dataOut, &bmp->data[rowStart], rowSize);
        dataOut += rowSize;
    }
}
