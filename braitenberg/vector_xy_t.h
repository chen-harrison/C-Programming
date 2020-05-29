#pragma once

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M_PI 3.14159265358979323846
#define WIDTH 640
#define HEIGHT 480

typedef struct pt {
    double x;
    double y;
} pt_t;

typedef struct vector {
    int cap;
    int size;
    pt_t *v;
} vector_t;

vector_t *vector_create(void);
void vector_expand(vector_t *vec);
void vector_append(vector_t *vec, pt_t newPoint);
void plot_edge(vector_t *vec, double x0, double y0, double x1, double y1);
void translate(vector_t *vec, double shiftX, double shiftY);
void rotate(vector_t *vec, double rad);
void fill_in(vector_t *vec, int rows);
void rounding(vector_t *vec);
int next(int i, int max);
vector_t *plot_rect(double width, double height, double rad,
                    double shiftX, double shiftY, bool fill);
vector_t *plot_tri(double width, double height, double rad,
                   double shiftX, double shiftY, bool fill);
double cross_products(int i, int j, vector_t *vecA, vector_t *vecB);
bool intersect(vector_t *robVertices, vector_t *lampVertices);
