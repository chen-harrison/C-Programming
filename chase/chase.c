#define _GNU_SOURCE

#include <unistd.h>
#include <time.h>
#include "bmp.h"
#include "robot_t.h"
#include "vector_xy_t.h"
#include "image_server.h"

color_bgr_t white = {255, 255, 255};
color_bgr_t green = {0, 255, 0};
color_bgr_t red = {0, 0, 255};
color_bgr_t black = {0, 0, 0};

typedef struct state {
    int time_step;
    bitmap_t bmp;
    size_t imageSize;
    uint8_t *imageData;
    bool runnerCaught;
    robot_t chase;
    robot_t run;
} state_t;

void state_init(state_t *state, int IRI) {
    state->bmp.width = MAP_W;
    state->bmp.height = MAP_H;
    state->bmp.data = calloc(state->bmp.width * state->bmp.height,
                             sizeof(color_bgr_t));
    state->imageSize = bmp_calculate_size(&state->bmp);
    state->imageData = malloc(state->imageSize);
    state->runnerCaught = false;
    state->chase.x = MAP_W / 2.0;
    state->chase.y = MAP_H / 2.0;
    int gridX = IRI % GRID_W;
    int gridY = IRI / GRID_W;
    state->run.x = (gridX + 0.5) * BLOCK_L;
    state->run.y = (gridY + 0.5) * BLOCK_L;
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
    vector_t *fill = plot_rect(MAP_W, MAP_H, 0.0, 0.0, 0.0, true);
    color_pixels(state, fill, white);
    for (int i = 0; i < GRID_W; i++) {
        for (int j = 0; j < GRID_H; j++) {
            int idx = (MAP_W / BLOCK_L) * j + i;
            if (MAP[idx] == 'X') {
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

void update_movement(state_t *state) {
    int randomAction = rand() % 20;
    runner_action(&state->run, randomAction);

    search_node_t current = {0};
    current.depth = 1;
    current.chase = state->chase;
    current.run = state->run;
    int chosenAction = 0;
    search_actions(current, &chosenAction);
    printf("%d\n", chosenAction);
    chaser_action(&state->chase, chosenAction);

    rob_time_step(&state->chase);
    rob_time_step(&state->run);
    if (chase_collision(&state->chase, &state->run)) {
        state->runnerCaught = true;
    }
    state->time_step++;
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s <time steps> <fast=0|1> <initial runner index>\n", argv[0]);
        return 1;
    }
    int steps;
    int fast;
    int IRI;
    int readSteps = sscanf(argv[1], "%d", &steps);
    int readFast = sscanf(argv[2], "%d", &fast);
    int readIRI = sscanf(argv[3], "%d", &IRI);
    if (!readSteps || !readFast || !readIRI) {
        fprintf(stderr, "<time steps>, <fast=0-2>, and/or <initial runner index> failed to read\n");
        return 1;
    }

    int seconds = 0;
    long nanoseconds = 40 * 1000 * 1000;
    struct timespec interval = {seconds, nanoseconds};

    state_t state = {0};
    state_init(&state, IRI);
    for (int i = 0; i < steps; i++) {
        update_movement(&state);

        if (!fast) {
            update_graphics(&state);
            nanosleep(&interval, NULL);
        }
        if (state.runnerCaught) {
            break;
        }
    }
    if (fast == 1 || (!fast && !steps)) {
        update_graphics(&state);
        sleep(1);
    }
    free(state.bmp.data);
    free(state.imageData);

    return 0;
}
