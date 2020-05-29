#include "vector_xy_t.h"

vector_t *vector_create(void) {
    vector_t *vec = malloc(sizeof(vector_t));
    vec->cap = 2;
    vec->size = 0;
    vec->v = malloc(sizeof(pt_t) * vec->cap);
    return vec;
}

void vector_expand(vector_t *vec) {
    while (vec->size >= vec->cap) {
        vec->cap *= 2;
        vec->v = realloc(vec->v, sizeof(pt_t) * vec->cap);
    }
}

void vector_append(vector_t *vec, pt_t newPoint) {
    vector_expand(vec);
    vec->v[vec->size] = newPoint;
    vec->size++;
}

void plot_edge(vector_t *vec, double x0, double y0, double x1, double y1) {
    double dx = fabs(x1 - x0);
    int sx;
    if (x0 < x1) {
        sx = 1;
    } else {
        sx = -1;
    }
    double dy = -fabs(y1 - y0);
    int sy;
    if (y0 < y1) {
        sy = 1;
    } else {
        sy = -1;
    }
    double err = dx + dy;
    pt_t newPoint = {x0, y0};
    while ((round(x0) != round(x1)) || (round(y0) != round(y1))) {
        vector_append(vec, newPoint);
        double e2 = 2 * err;
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
}

void translate(vector_t *vec, double shiftX, double shiftY) {
    for (int i = 0; i < vec->size; i++) {
        vec->v[i].x += shiftX;
        vec->v[i].y += shiftY;
    }
}

void rotate(vector_t *vec, double rad) {
    for (int i = 0; i < vec->size; i++) {
        double x = vec->v[i].x;
        double y = vec->v[i].y;
        vec->v[i].x = x * cos(rad) - y * sin(rad);
        vec->v[i].y = x * sin(rad) + y * cos(rad);
    }
}

void fill_in(vector_t *vec, int rows) {
    int x0[rows];
    int x1[rows];
    for (int h = 0; h < rows; h++) {
        x0[h] = -1;
        x1[h] = -1;
    }

    for (int i = 0; i < vec->size; i++) {
        int currentY = (int)vec->v[i].y;
        if (x0[currentY] == -1) {
            x0[currentY] = (int)vec->v[i].x;
            x1[currentY] = (int)vec->v[i].x;
        } else {
            if (vec->v[i].x < x0[currentY]) {
                x0[currentY] = (int)vec->v[i].x;
            }
            if (vec->v[i].x > x1[currentY]) {
                x1[currentY] = (int)vec->v[i].x;
            }
        }
    }
    for (int j = 0; j < rows; j++) {
        if (x0[j] != x1[j]) {
            for (int k = x0[j] + 1; k < x1[j]; k++) {
                pt_t newPoint = {0};
                newPoint.x = (double)k;
                newPoint.y = (double)j;
                vector_append(vec, newPoint);
            }
        }
    }
}

void rounding(vector_t *vec) {
    double minX = vec->v[0].x;
    double minY = vec->v[0].y;
    for (int i = 0; i < vec->size; i++) {
        if (vec->v[i].x < minX) {
            minX = vec->v[i].x;
        }
        if (vec->v[i].y < minY) {
            minY = vec->v[i].y;
        }
    }

    double ep = pow(10, -6);
    for (int j = 0; j < vec->size; j++) {
        if (vec->v[j].x == minX) {
            if (ceil(vec->v[j].x) != ceil(vec->v[j].x - ep)) {
                vec->v[j].x = round(vec->v[j].x);
            } else {
                vec->v[j].x = ceil(vec->v[j].x);
            }
        } else {
            vec->v[j].x = floor(vec->v[j].x - ep);
        }
        if (vec->v[j].y == minY) {
            if (ceil(vec->v[j].y) != ceil(vec->v[j].y - ep)) {
                vec->v[j].y = round(vec->v[j].y);
            } else {
                vec->v[j].y = ceil(vec->v[j].y);
            }
        } else {
            vec->v[j].y = floor(vec->v[j].y - ep);
        }
    }
}

int next(int i, int max) {
    if (i + 1 == max) {
        return 0;
    }
    return i + 1;
}

vector_t *plot_rect(double width, double height, double rad,
                    double shiftX, double shiftY, bool fill) {
    double x = width / 2.0;
    double y = height / 2.0;
    pt_t hold[4] = {{-x, -y}, {-x, y}, {x, y}, {x, -y}};
    vector_t *vertices = vector_create();
    for (int i = 0; i < 4; i++) {
        vector_append(vertices, hold[i]);
    }
    rotate(vertices, rad);
    translate(vertices, shiftX, shiftY);
    rounding(vertices);
    vector_t *vec = vector_create();
    for (int i = 0; i < 4; i++) {
        int ii = next(i, 4);
        pt_t start = vertices->v[i];
        pt_t end = vertices->v[ii];
        plot_edge(vec, start.x, start.y, end.x, end.y);
    }
    if (fill) {
        fill_in(vec, HEIGHT);
    }
    free(vertices->v);
    free(vertices);
    return vec;
}

vector_t *plot_tri(double width, double height, double rad,
                   double shiftX, double shiftY, bool fill) {
    double x = width / 2.0;
    double y = height / 2.0;
    pt_t hold[3] = {{-x, y}, {-x, -y}, {x, 0.0}};
    vector_t *vertices = vector_create();
    for (int i = 0; i < 3; i++) {
        vector_append(vertices, hold[i]);
    }
    rotate(vertices, rad);
    translate(vertices, shiftX, shiftY);
    rounding(vertices);
    vector_t *vec = vector_create();
    for (int i = 0; i < 3; i++) {
        int ii = next(i, 3);
        pt_t start = vertices->v[i];
        pt_t end = vertices->v[ii];
        plot_edge(vec, start.x, start.y, end.x, end.y);
    }
    if (fill) {
        fill_in(vec, HEIGHT);
    }
    free(vertices->v);
    free(vertices);
    return vec;
}

double cross_products(int i, int j, vector_t *vecA, vector_t *vecB) {
    int ii = next(i, vecA->size);
    int jj = next(j, vecB->size);

    pt_t startA = vecA->v[i];
    pt_t endA = vecA->v[ii];
    pt_t startB = vecB->v[j];
    pt_t endB = vecB->v[jj];
    pt_t vecAA = {endA.x - startA.x, endA.y - startA.y};
    pt_t vecAB1 = {startB.x - startA.x, startB.y - startA.y};
    pt_t vecAB2 = {endB.x - startA.x, endB.y - startA.y};

    double cross1 = vecAA.x * vecAB1.y - vecAA.y * vecAB1.x;
    double cross2 = vecAA.x * vecAB2.y - vecAA.y * vecAB2.x;
    return cross1 * cross2;
}

bool intersect(vector_t *robVertices, vector_t *lampVertices) {
    bool collided = false;
    for (int i = 0; i < robVertices->size; i++) {
        for (int j = 0; j < lampVertices->size; j++) {
            double crosses1 = cross_products(i, j, robVertices, lampVertices);
            double crosses2 = cross_products(j, i, lampVertices, robVertices);
            if ((crosses1 <= 0 && crosses2 < 0) ||
                (crosses1 < 0 && crosses2 <= 0)) {
                collided = true;
                break;
            }
        }
        if (collided) {
            break;
        }
    }
    return collided;
}
