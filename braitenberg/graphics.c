#include "graphics.h"

color_bgr_t white = {255, 255, 255};
color_bgr_t yellow = {0, 255, 255};
color_bgr_t green = {0, 255, 0};
color_bgr_t black = {0, 0, 0};
double lampX[3] = {124.1, 349.1, 449.1};
double lampY[3] = {224.1, 99.1, 349.1};

void color_pixels(bitmap_t *bmp, vector_t *vec, color_bgr_t color) {
    for (int i = 0; i < vec->size; i++) {
        if (vec->v[i].x >= 0 && vec->v[i].y >= 0) {
            int pixel = (int)(vec->v[i].y * bmp->width + vec->v[i].x);
            bmp->data[pixel] = color;
        }
    }
    free(vec->v);
    free(vec);
}

void add_lamps(bitmap_t *bmp) {
    for (int i = 0; i < 3; i++) {
        vector_t *lamp = plot_rect(LAMP_L, LAMP_L, M_PI / 4.0, lampX[i], lampY[i], true);
        color_pixels(bmp, lamp, yellow);
    }
}

vector_t *lamp_vertices(double x, double y) {
    double l = LAMP_L / 2.0;
    pt_t hold[4] = {{-l, -l}, {l, -l}, {l, l}, {-l, l}};
    vector_t *vertices = vector_create();
    for (int i = 0; i < 4; i++) {
        vector_append(vertices, hold[i]);
    }
    rotate(vertices, M_PI / 4.0);
    translate(vertices, x, y);
    return vertices;
}

void image_init(bitmap_t *bmp) {
    vector_t *border = plot_rect(600, 440, 0, 320, 240, false);
    color_pixels(bmp, border, white);
    add_lamps(bmp);
}

void image_finish(bitmap_t *bmp, uint8_t *bmpSerialized, size_t bmpSize) {
    bmp_serialize(bmp, bmpSerialized);
    FILE *f = fopen("my_image.bmp", "wb");
    fwrite(bmpSerialized, bmpSize, 1, f);
    fclose(f);
    image_server_set_data(bmpSize, bmpSerialized);
    image_server_start("8000");
}
