#include "vector_xy_i32_t.h"

vector_i32_t *vector_i32_create(void) {
    vector_i32_t *vec = malloc(sizeof(vector_i32_t));
    vec->cap = 2;
    vec->size = 0;
    vec->v = malloc(sizeof(pt_i32_t) * vec->cap);
    return vec;
}

void vector_i32_expand(vector_i32_t *vec) {
    while (vec->size == vec->cap) {
        vec->cap *= 2;
        vec->v = realloc(vec->v, sizeof(pt_i32_t) * vec->cap);
    }
}

void vector_i32_append(vector_i32_t *vec, pt_i32_t newPoint) {
    vector_i32_expand(vec);
    vec->v[vec->size] = newPoint;
    vec->size++;
}

vector_i32_t *plotLine_i32(int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int sx;
    if (x0 < x1) {
        sx = 1;
    } else {
        sx = -1;
    }
    int dy = -abs(y1 - y0);
    int sy;
    if (y0 < y1) {
        sy = 1;
    } else {
        sy = -1;
    }
    int err = dx + dy;
    vector_i32_t *vec = vector_i32_create();
    pt_i32_t newPoint = {x0, y0};
    while ((x0 != x1) || (y0 != y1)) {
        vector_i32_append(vec, newPoint);
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
        newPoint.x = x0;
        newPoint.y = y0;
    }
    vector_i32_append(vec, newPoint);
    return vec;
}
