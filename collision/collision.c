#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define M_PI 3.14159265358979323846
#define MAX_VERTICES 16

typedef struct shape {
    double shiftX;
    double shiftY;
    double rot;
    int points;
    double x[MAX_VERTICES];
    double y[MAX_VERTICES];
} shape_t;

bool read_shape(FILE *f, shape_t *poly) {
    int argsRead = fscanf(f, "%lf %lf %lf %d", &poly->shiftX, &poly->shiftY,
                          &poly->rot, &poly->points);
    if (argsRead < 4) {
        fprintf(stderr, "One of the x, y, rot, or n_points values is invalid.\n");
        return true;
    }
    if (poly->points > MAX_VERTICES) {
        fprintf(stderr, "Number of vertices in the first polygon exceeds the limit of %d.\n",
                MAX_VERTICES);
        return true;
    }
    for (int i = 0; i < poly->points; i++) {
        argsRead = fscanf(f, "%lf", &poly->x[i]);
        if (!argsRead) {
            fprintf(stderr, "A given x-coordinate is invalid.\n");
            return true;
        }
    }
    for (int i = 0; i < poly->points; i++) {
        argsRead = fscanf(f, "%lf", &poly->y[i]);
        if (!argsRead) {
            fprintf(stderr, "A given y-coordinate is invalid.\n");
            return true;
        }
    }
    return false;
}

void rot_shift(shape_t *poly) {
    double rad = M_PI / 180.0 * poly->rot;
    for (int i = 0; i < poly->points; i++) {
        double x = poly->x[i];
        double y = poly->y[i];
        poly->x[i] = x * cos(rad) + y * -sin(rad) + poly->shiftX;
        poly->y[i] = x * sin(rad) + y * cos(rad) + poly->shiftY;
    }
}

int next(int i, int max) {
    if (i + 1 == max) {
        return 0;
    }
    return i + 1;
}

double cross_products(int i, int j, shape_t *polyA, shape_t *polyB) {
    int ii = next(i, polyA->points);
    int jj = next(j, polyB->points);
    // start and end points of edges along polyA and polyB
    double startA[2] = {polyA->x[i], polyA->y[i]};
    double endA[2] = {polyA->x[ii], polyA->y[ii]};
    double startB[2] = {polyB->x[j], polyB->y[j]};
    double endB[2] = {polyB->x[jj], polyB->y[jj]};
    // vectors from start of polyA edge to all three other points
    double vecAA[2] = {endA[0] - startA[0],
                       endA[1] - startA[1]};
    double vecAB1[2] = {startB[0] - startA[0],
                        startB[1] - startA[1]};
    double vecAB2[2] = {endB[0] - startA[0],
                        endB[1] - startA[1]};

    double cross1 = vecAA[0] * vecAB1[1] - vecAA[1] * vecAB1[0];
    double cross2 = vecAA[0] * vecAB2[1] - vecAA[1] * vecAB2[0];

    return cross1 * cross2;
}

bool inside(shape_t *poly, double ptX, double ptY) {
    for (int i = 0; i < poly->points; i++) {
        int ii = next(i, poly->points);
        int iii = next(ii, poly->points);
        double outer1[2] = {poly->x[ii] - poly->x[i], poly->y[ii] - poly->y[i]};
        double inner1[2] = {ptX - poly->x[i], ptY - poly->y[i]};
        double outer2[2] = {poly->x[iii] - poly->x[ii], poly->y[iii] - poly->y[ii]};
        double inner2[2] = {ptX - poly->x[ii], ptY - poly->y[ii]};
        double cross1 = outer1[0] * inner1[1] - outer1[1] * inner1[0];
        double cross2 = outer2[0] * inner2[1] - outer2[1] * inner2[0];
        if (cross1 * cross2 < 0) {
            return false;
        }
    }
    return true;
}

int main(void) {
    shape_t poly1 = {0};
    shape_t poly2 = {0};
    FILE *f = fopen("polygons.csv", "r");
    if (!f) {
        fprintf(stderr, "polygons.csv is missing.\n");
        return 1;
    }
    // skip past the first line, which holds the labels
    char firstLine[256];
    fgets(firstLine, 256, f);
    // if error is encountered while reading in values, exit
    if (read_shape(f, &poly1) || read_shape(f, &poly2)) {
        return 1;
    }
    rot_shift(&poly1);
    rot_shift(&poly2);
    bool collided = false;
    for (int i = 0; i < poly1.points; i++) {
        for (int j = 0; j < poly2.points; j++) {
            double crosses1 = cross_products(i, j, &poly1, &poly2);
            double crosses2 = cross_products(j, i, &poly2, &poly1);
            // zero, zero   = parallel edges
            // zero, neg    = vertex on edge
            // neg, neg     = vectors intersect
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
    // if no collision found via intersection, check inside condition
    if (!collided) {
        if (inside(&poly1, poly2.x[0], poly2.y[0]) ||
            inside(&poly2, poly1.x[0], poly1.y[0])) {
            collided = true;
        }
    }
    if (collided) {
        printf("collision!\n");
    } else {
        printf("no collision\n");
    }
    return 0;
}
