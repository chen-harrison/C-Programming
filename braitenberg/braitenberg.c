#define _GNU_SOURCE

#include <unistd.h>
#include <time.h>
#include "graphics.h"

#define SIXTY (M_PI / 3.0)
#define ROB_W 21.0
#define ROB_L 28.0
#define START_X 320.0
#define START_Y 240.0
#define POWER 100000.0
#define MAX_MOVE 12.0
#define WHEEL_BASE 80.0

typedef struct robot {
    double x;
    double y;
    double rad;
} robot_t;

double max(const double a, const double b) {
    if (b > a) {
        return b;
    }
    return a;
}

double min(const double a, const double b) {
    if (b < a) {
        return b;
    }
    return a;
}

double inner(const double *v1, const double *v2) {
    return v1[0] * v2[0] + v1[1] * v2[1];
}

void rob_iterate(robot_t *rob) {
    double moveL = 0;
    double moveR = 0;

    for (int i = 0; i < 3; i++) {
        double distSq = pow(lampX[i] - rob->x, 2) + pow(lampY[i] - rob->y, 2);
        double dir[2] = {(lampX[i] - rob->x) / sqrt(distSq),
                         (lampY[i] - rob->y) / sqrt(distSq)};
        double eyeL[2] = {cos(rob->rad - SIXTY), sin(rob->rad - SIXTY)};
        double eyeR[2] = {cos(rob->rad + SIXTY), sin(rob->rad + SIXTY)};
        moveL += max(0.0, inner(dir, eyeR)) / distSq * POWER;
        moveR += max(0.0, inner(dir, eyeL)) / distSq * POWER;
    }

    moveL = min(MAX_MOVE, moveL);
    moveR = min(MAX_MOVE, moveR);
    rob->rad += (moveL - moveR) / WHEEL_BASE;
    double forwardDist = (moveR + moveL) / 2.0;
    rob->x += forwardDist * cos(rob->rad);
    rob->y += forwardDist * sin(rob->rad);
}

int full_collision(robot_t *rob) {
    pt_t hold[3] = {{-ROB_L / 2.0, ROB_W / 2.0}, {-ROB_L / 2.0, -ROB_W / 2.0}, {ROB_L / 2.0, 0.0}};
    vector_t *robVertices = vector_create();
    for (int i = 0; i < 3; i++) {
        vector_append(robVertices, hold[i]);
    }
    rotate(robVertices, rob->rad);
    translate(robVertices, rob->x, rob->y);

    int out = -1;
    for (int i = 0; i < 3; i++) {
        vector_t *lampVertices = lamp_vertices(lampX[i], lampY[i]);
        if (intersect(robVertices, lampVertices)) {
            out = i;
        }
        free(lampVertices->v);
        free(lampVertices);
    }
    free(robVertices->v);
    free(robVertices);
    return out;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <time steps> <fast=0|1>\n", argv[0]);
        return 1;
    }
    int steps;
    int fast;
    int readSteps = sscanf(argv[1], "%d", &steps);
    int readFast = sscanf(argv[2], "%d", &fast);
    if (!readSteps || !readFast) {
        fprintf(stderr, "<time steps> and/or <fast=0|1> failed to read\n");
        return 1;
    }

    bitmap_t bmp = {0};
    bmp.width = WIDTH;
    bmp.height = HEIGHT;
    bmp.data = calloc(bmp.width * bmp.height, sizeof(color_bgr_t));
    size_t bmpSize = bmp_calculate_size(&bmp);
    uint8_t *bmpSerialized = malloc(bmpSize);
    image_init(&bmp);

    robot_t rob = {START_X, START_Y, 0.0};
    vector_t *robStep = plot_tri(ROB_L, ROB_W, 0.0, rob.x, rob.y, true);
    int seconds = 0;
    long nanoseconds = 40 * 1000 * 1000;
    struct timespec interval = {seconds, nanoseconds};

    for (int i = 0; i < steps; i++) {
        rob_iterate(&rob);

        int c = full_collision(&rob);
        while (c > -1) {
            // if collided with a lamp, resolve
            double pathLength = sqrt(pow(rob.x - lampX[c], 2) +
                                     pow(rob.y - lampY[c], 2));
            double path[2] = {(rob.x - lampX[c]) / pathLength,
                              (rob.y - lampY[c]) / pathLength};
            rob.x += path[0] * 0.5;
            rob.y += path[1] * 0.5;
            c = full_collision(&rob);
        }

        if (!fast) {
            color_pixels(&bmp, robStep, black);
            add_lamps(&bmp);
            robStep = plot_tri(ROB_L, ROB_W, rob.rad, rob.x, rob.y, true);
            color_pixels(&bmp, robStep, green);
            robStep = plot_tri(ROB_L, ROB_W, rob.rad, rob.x, rob.y, true);
            image_finish(&bmp, bmpSerialized, bmpSize);
            nanosleep(&interval, NULL);
        }
    }
    free(robStep->v);
    free(robStep);

    if (fast) {
        vector_t *robFinal = plot_tri(ROB_L, ROB_W, rob.rad, rob.x, rob.y, true);
        color_pixels(&bmp, robFinal, green);
        image_finish(&bmp, bmpSerialized, bmpSize);
        sleep(1);
    } else if (!fast && !steps) {
        vector_t *robStart = plot_tri(ROB_L, ROB_W, 0.0, START_X, START_Y, true);
        color_pixels(&bmp, robStart, green);
        image_finish(&bmp, bmpSerialized, bmpSize);
        nanosleep(&interval, NULL);
    }
    free(bmp.data);
    free(bmpSerialized);
    return 0;
}
