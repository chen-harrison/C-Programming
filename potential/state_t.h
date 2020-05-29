#pragma once

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#include "vector_xy_t.h"
#include "robot_t.h"
#include "image_server.h"

#define HIGHLIGHT "\e[7m"
#define CLEAR_HIGHLIGHT "\e[0m"
#define ROB_R (sqrt(pow(ROB_H / 2.0, 2) + pow(ROB_W / 2.0, 2)))
#define WALL_R (BLOCK_L / sqrt(2.0))

color_bgr_t white;
color_bgr_t yellow;
color_bgr_t green;
color_bgr_t red;
color_bgr_t black;

typedef struct state {
    int IRI;
    int delayEvery;
    double goalMag;
    int goalPow;
    double obsMag;
    int obsPow;
    int maxVelocity;

    bool disable;
    int key;
    int time_step;
    bitmap_t bmp;
    size_t imageSize;
    uint8_t *imageData;
    bool runnerCaught;
    robot_t chase;
    robot_t run;
} state_t;

void state_init(state_t *state);
void state_reset(state_t *state);
void color_pixels(state_t *state, vector_t *vec, color_bgr_t color);
void update_graphics(state_t *state);
double check_bounds(double val, double lb, double ub);
void adjust_parameters(state_t *state, int sign);
void print_interface(state_t *state);
void runner_action(state_t *state);
void chaser_action(state_t *state);
void update_movement(state_t *state);
