#include "bmp.h"
#include "image_server.h"
#include "vector_xy_t.h"

#define LAMP_L 12.0

color_bgr_t white;
color_bgr_t yellow;
color_bgr_t green;
color_bgr_t black;
double lampX[3];
double lampY[3];

void color_pixels(bitmap_t *bmp, vector_t *vec, color_bgr_t color);
void add_lamps(bitmap_t *bmp);
vector_t *lamp_vertices(double x, double y);
void image_init(bitmap_t *bmp);
void image_finish(bitmap_t *bmp, uint8_t *bmpSerialized, size_t bmpSize);
