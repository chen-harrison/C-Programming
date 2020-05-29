#pragma once

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vector_xy_t.h"

#define BLOCK_L 40
#define GRID_W (MAP_W / BLOCK_L)
#define GRID_H (MAP_H / BLOCK_L)
#define ROB_W 20.0
#define ROB_H 26.67
#define MAP "XXXXXXXXXXXXXXXX" \
            "X              X" \
            "X  XXXX   XXX  X" \
            "X   XX      X  X" \
            "X       XXX    X" \
            "XXXXXX         X" \
            "X         XXXXXX" \
            "X    XXX       X" \
            "X  XX     XX   X" \
            "X   X    XX    X" \
            "X      XXX     X" \
            "XXXXXXXXXXXXXXXX"

typedef struct robot {
    double x;
    double y;
    double theta;
    double v;
    double omega;
} robot_t;

vector_t *rob_v(robot_t *rob);
bool chase_collision(robot_t *chase, robot_t *run);
vector_t *block_v(int x, int y);
bool block_collision(robot_t *rob);
void rob_time_step(robot_t *rob);
