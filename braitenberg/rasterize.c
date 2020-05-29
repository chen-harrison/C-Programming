#include <unistd.h>
#include "bmp.h"
#include "image_server.h"
#include "vector_xy_t.h"
#include "vector_xy_i32_t.h"

void color_pixels(bitmap_t *bmp, vector_t *vec) {
    for (int i = 0; i < vec->size; i++) {
        if (vec->v[i].x >= 0 && vec->v[i].y >= 0) {
            int pixel = (int)(vec->v[i].y * bmp->width + vec->v[i].x);
            bmp->data[pixel].r = 255;
            bmp->data[pixel].g = 255;
            bmp->data[pixel].b = 255;
        }
    }
    free(vec->v);
    free(vec);
}

void image_finish(bitmap_t *bmp, uint8_t *bmpSerialized, size_t bmpSize) {
    bmp_serialize(bmp, bmpSerialized);
    FILE *f = fopen("my_image.bmp", "wb");
    fwrite(bmpSerialized, bmpSize, 1, f);
    fclose(f);
    image_server_set_data(bmpSize, bmpSerialized);
    image_server_start("8000");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <test case>\n", argv[0]);
        return 1;
    }
    int test;
    int argsRead = sscanf(argv[1], "%d", &test);
    if (!argsRead) {
        fprintf(stderr, "invalid: <test case> must be type int\n");
        return 1;
    }
    bitmap_t bmp = {0};
    bmp.width = WIDTH;
    bmp.height = HEIGHT;
    bmp.data = calloc(bmp.width * bmp.height, sizeof(color_bgr_t));
    // REMINDER! in bmp.data, pixel data is in its natural order
    // bmpSerialized is what takes these and reorders them
    size_t bmpSize = bmp_calculate_size(&bmp);
    uint8_t *bmpSerialized = malloc(bmpSize);
    if (test == 2) {
        vector_i32_t *vec2 = plotLine_i32(10, 10, 200, 200);
        for (int i = 0; i < vec2->size; i++) {
            int pixel = vec2->v[i].y * bmp.width + vec2->v[i].x;
            bmp.data[pixel].r = 255;
            bmp.data[pixel].g = 255;
            bmp.data[pixel].b = 255;
        }
        free(vec2->v);
        free(vec2);
    } else if (test == 3 || test == 4) {
        vector_t *vec3 = plot_rect(4, 4, 0, 0, 0, false);
        if (test == 4) {
            translate(vec3, 2, 2);
        }
        color_pixels(&bmp, vec3);
    } else if (test == 5) {
        vector_t *vec5 = plot_rect(5, 5, 0, 2, 2, false);
        color_pixels(&bmp, vec5);
    } else if (test == 6 || test == 7) {
        vector_t *vec6 = plot_rect(600, 440, 0, 320, 240, false);
        if (test == 7) {
            fill_in(vec6, HEIGHT);
        }
        color_pixels(&bmp, vec6);
    } else if (test == 8) {
        vector_t *vec8 = plot_tri(28, 21, 0, 400, 400, true);
        color_pixels(&bmp, vec8);
    } else if (test == 9) {
        vector_t *vec9 = plot_tri(28, 21, -M_PI / 6, 400, 400, true);
        color_pixels(&bmp, vec9);
    }
    image_finish(&bmp, bmpSerialized, bmpSize);
    free(bmp.data);
    free(bmpSerialized);
    sleep(1);
    return 0;
}
