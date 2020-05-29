#pragma once

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "vector_xy_t.h"

#define BLOCK_L 40
#define GRID_W (MAP_W / BLOCK_L)
#define GRID_H (MAP_H / BLOCK_L)
#define ROB_W 20.0
#define ROB_H 26.67
#define MAX_DEPTH 4
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

typedef struct search_node {
    int depth;
    robot_t chase;
    robot_t run;
} search_node_t;

vector_t *rob_v(robot_t *rob);
bool chase_collision(robot_t *chase, robot_t *run);
vector_t *block_v(int x, int y);
bool block_collision(robot_t *rob);
void runner_action(robot_t *run, int action);
void chaser_action(robot_t *chase, int action);
void rob_time_step(robot_t *rob);
double chaser_sim(search_node_t node, int *chosenAction, int test);
double search_actions(search_node_t node, int *chosenAction);
