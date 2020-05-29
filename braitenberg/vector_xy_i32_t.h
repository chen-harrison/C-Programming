#pragma once

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct pt_i32 {
    int x;
    int y;
} pt_i32_t;

typedef struct vector_i32 {
    int cap;
    int size;
    pt_i32_t *v;
} vector_i32_t;

vector_i32_t *vector_i32_create(void);
void vector_i32_expand(vector_i32_t *vec);
void vector_i32_append(vector_i32_t *vec, pt_i32_t newPoint);
vector_i32_t *plotLine_i32(int x0, int y0, int x1, int y1);
