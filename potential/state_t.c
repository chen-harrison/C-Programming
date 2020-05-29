#define _GNU_SOURCE

#include <unistd.h>
#include <time.h>
#include "state_t.h"

color_bgr_t white = {255, 255, 255};
color_bgr_t yellow = {0, 255, 255};
color_bgr_t green = {0, 255, 0};
color_bgr_t red = {0, 0, 255};
color_bgr_t black = {0, 0, 0};

void state_init(state_t *state) {
    state->IRI = 17;
    state->delayEvery = 1;
    state->goalMag = 50.0;
    state->goalPow = 0;
    state->obsMag = 1.0;
    state->obsPow = -2;
    state->maxVelocity = 12;

    state->disable = false;
    state->key = 1;
    state->time_step = 0;
    state->bmp.width = MAP_W;
    state->bmp.height = MAP_H;
    state->bmp.data = calloc(state->bmp.width * state->bmp.height,
                             sizeof(color_bgr_t));
    state->imageSize = bmp_calculate_size(&state->bmp);
    state->imageData = malloc(state->imageSize);

    state->runnerCaught = false;
    state->chase.x = MAP_W / 2.0;
    state->chase.y = MAP_H / 2.0;
    int map_idx_x = state->IRI % GRID_W;
    int map_idx_y = state->IRI / GRID_W;
    state->run.x = (map_idx_x + 0.5) * BLOCK_L;
    state->run.y = (map_idx_y + 0.5) * BLOCK_L;
}

void state_reset(state_t *state) {
    int seconds = 0;
    long nanoseconds = 40000 * 1000;
    struct timespec interval = {seconds, nanoseconds};
    srand(0);
    state->runnerCaught = false;
    state->time_step = 0;
    state->chase.x = MAP_W / 2.0;
    state->chase.y = MAP_H / 2.0;
    int map_idx_x = state->IRI % GRID_W;
    int map_idx_y = state->IRI / GRID_W;
    state->run.x = (map_idx_x + 0.5) * BLOCK_L;
    state->run.y = (map_idx_y + 0.5) * BLOCK_L;
    state->chase.v = 0;
    state->run.v = 0;
    state->chase.omega = 0;
    state->run.omega = 0;
    state->chase.theta = 0;
    state->run.theta = 0;
    if (!state->disable) {
        update_graphics(state);
        nanosleep(&interval, NULL);
    }
}

void color_pixels(state_t *state, vector_t *vec, color_bgr_t color) {
    for (int i = 0; i < vec->size; i++) {
        if (vec->v[i].x >= 0 && vec->v[i].y >= 0) {
            int pixel = (int)(vec->v[i].y * state->bmp.width + vec->v[i].x);
            state->bmp.data[pixel] = color;
        }
    }
    free(vec->v);
    free(vec);
}

void update_graphics(state_t *state) {
    vector_t *fill = plot_rect(MAP_W, MAP_H, 0, 0, 0, true);
    color_pixels(state, fill, white);
    for (int i = 0; i < GRID_W; i++) {
        for (int j = 0; j < GRID_H; j++) {
            int index = 16 * j + i;
            if (MAP[index] == 'X') {
                vector_t *block = plot_rect(BLOCK_L, BLOCK_L, 0,
                                            BLOCK_L * i, BLOCK_L * j, true);
                color_pixels(state, block, black);
            }
        }
    }
    vector_t *chaser = plot_tri(ROB_H, ROB_W, state->chase.theta,
                                state->chase.x, state->chase.y, true);
    color_pixels(state, chaser, red);
    vector_t *runner = plot_tri(ROB_H, ROB_W, state->run.theta,
                                state->run.x, state->run.y, true);
    color_pixels(state, runner, green);

    bmp_serialize(&state->bmp, state->imageData);
    FILE *f = fopen("my_image.bmp", "wb");
    fwrite(state->imageData, state->imageSize, 1, f);
    fclose(f);
    image_server_set_data(state->imageSize, state->imageData);
    image_server_start("8000");
}

double check_bounds(double val, double lb, double ub) {
    if (val > ub) {
        val = ub;
    } else if (val < lb) {
        val = lb;
    }
    return val;
}

void adjust_parameters(state_t *state, int sign) {
    // key represents which parameter is being tuned
    // sign determines increase or decrease
    switch (state->key) {
    case 1:
        state->IRI += sign;
        while (MAP[state->IRI] == 'X') {
            state->IRI += sign;
            if (state->IRI < 0) {
                state->IRI = GRID_W * GRID_H - 1;
            } else if (state->IRI > GRID_W * GRID_H - 1) {
                state->IRI = 0;
            }
        }
        break;
    case 2:
        state->delayEvery += sign;
        if (state->delayEvery <= 0) {
            state->delayEvery = 1;
        }
        break;
    case 3:
        state->goalMag *= pow(2.0, (double)sign);
        break;
    case 4:
        state->goalPow += sign;
        state->goalPow = (int)check_bounds(state->goalPow, -3.0, 3.0);
        break;
    case 5:
        state->obsMag *= pow(2.0, (double)sign);
        break;
    case 6:
        state->obsPow += sign;
        state->obsPow = (int)check_bounds(state->obsPow, -3.0, 3.0);
        break;
    case 7:
        state->maxVelocity += sign;
        state->maxVelocity = (int)check_bounds(state->maxVelocity, 1.0, 12.0);
    default:
        break;
    }
}

void print_interface(state_t *state) {
    printf("\r");
    printf("%s%8d%s", (state->key == 1) ? HIGHLIGHT : "", state->IRI, CLEAR_HIGHLIGHT);
    printf("%s%8d%s", (state->key == 2) ? HIGHLIGHT : "", state->delayEvery, CLEAR_HIGHLIGHT);
    printf("%s%8.2f%s", (state->key == 3) ? HIGHLIGHT : "", state->goalMag, CLEAR_HIGHLIGHT);
    printf("%s%8d%s", (state->key == 4) ? HIGHLIGHT : "", state->goalPow, CLEAR_HIGHLIGHT);
    printf("%s%8.2f%s", (state->key == 5) ? HIGHLIGHT : "", state->obsMag, CLEAR_HIGHLIGHT);
    printf("%s%8d%s", (state->key == 6) ? HIGHLIGHT : "", state->obsPow, CLEAR_HIGHLIGHT);
    printf("%s%8d%s", (state->key == 7) ? HIGHLIGHT : "", state->maxVelocity, CLEAR_HIGHLIGHT);
    fflush(stdout);
}

void runner_action(state_t *state) {
    int act = rand() % 20;
    if (act == 1) {
        state->run.v += 2;
        if (state->run.v > 12) {
            state->run.v = 12;
        }
    } else if (act == 2) {
        state->run.omega -= M_PI / 16;
    }
}

void chaser_action(state_t *state) {
    pt_t force = {0.0, 0.0};
    // forces applied by goal
    double goalDist = sqrt(pow(state->run.x - state->chase.x, 2.0) +
                           pow(state->run.y - state->chase.y, 2.0));
    pt_t toGoal = {(state->run.x - state->chase.x) / goalDist,
                   (state->run.y - state->chase.y) / goalDist};
    force.x += toGoal.x * state->goalMag * pow(goalDist, (double)state->goalPow);
    force.y += toGoal.y * state->goalMag * pow(goalDist, (double)state->goalPow);
    // forces applied by obstacles
    for (int i = 0; i < GRID_W; i++) {
        for (int j = 0; j < GRID_H; j++) {
            int pos = 16 * j + i;
            if (MAP[pos] == 'X') {
                vector_t *blockVertices = block_v(i, j);
                pt_t blockCenter = vector_avg(blockVertices);
                free(blockVertices->v);
                free(blockVertices);
                double chaseDist = sqrt(pow(state->chase.x - blockCenter.x, 2.0) +
                                        pow(state->chase.y - blockCenter.y, 2.0));
                pt_t chaseVec = {(state->chase.x - blockCenter.x) / chaseDist,
                                 (state->chase.y - blockCenter.y) / chaseDist};
                double radiusDist = fmax(0.1, chaseDist - (ROB_R + WALL_R));
                force.x += chaseVec.x * state->obsMag * pow(radiusDist, (double)state->obsPow);
                force.y += chaseVec.y * state->obsMag * pow(radiusDist, (double)state->obsPow);
            }
        }
    }

    double thetaTarget = atan2(force.y, force.x);
    double thetaError = thetaTarget - state->chase.theta;
    // wrap to pi
    while (fabs(thetaError) > M_PI) {
        if (thetaError > 0) {
            thetaError -= 2 * M_PI;
        } else if (thetaError < 0) {
            thetaError += 2 * M_PI;
        }
    }
    state->chase.omega = check_bounds(0.4 * thetaError, -M_PI / 16.0, M_PI / 16.0);
    state->chase.v = fmin((double)state->maxVelocity, state->chase.v + 2.0);
}

void update_movement(state_t *state) {
    chaser_action(state);
    runner_action(state);
    rob_time_step(&state->chase);
    rob_time_step(&state->run);
    if (chase_collision(&state->chase, &state->run)) {
        state->runnerCaught = true;
    }
    state->time_step++;
}
