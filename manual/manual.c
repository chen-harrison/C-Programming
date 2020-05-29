#define _GNU_SOURCE

#include <unistd.h>
#include <time.h>
#include <termios.h>
#include <pthread.h>
#include "bmp.h"
#include "image_server.h"
#include "vector_xy_t.h"
#include "robot_t.h"

color_bgr_t white = {255, 255, 255};
color_bgr_t yellow = {0, 255, 255};
color_bgr_t green = {0, 255, 0};
color_bgr_t red = {0, 0, 255};
color_bgr_t black = {0, 0, 0};

typedef struct state {
    int timeStep;
    bitmap_t bmp;
    size_t imageSize;
    uint8_t *imageData;
    robot_t run;
    int userAction;
} state_t;

struct termios originalTermios;
void reset_terminal(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
}

void *io_thread(void *user) {
    state_t *state = user;

    // we need to return the terminal to its original state when our program quits
    // if we don't the terminal will not behave correctly
    tcgetattr(0, &originalTermios);
    atexit(reset_terminal);

    // we start with the old settings, since we are only changing a few things
    struct termios newTermios;
    memcpy(&newTermios, &originalTermios, sizeof(newTermios));

    // we turn off the bits for echo (show typed characters on the screen)
    // and canonical mode, which waits until you press newline to give the program input
    newTermios.c_lflag &= ~(ECHO | ICANON);
    // we are modifying the input settings, (standard in, stdin)
    // and we want the changes to take effect right now!
    tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);

    while (true) {
        int c = getc(stdin);
        if (c == 'q') {
            // QUIT
            exit(0);
        } else if (c == 27) {
            if (getc(stdin) == '[') {
                switch (getc(stdin)) {
                case 65:
                    // UP
                    state->userAction = 1;
                    break;
                case 68:
                    // LEFT
                    state->userAction = 2;
                    break;
                case 67:
                    // RIGHT
                    state->userAction = 3;
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void state_init(state_t *state) {
    state->bmp.width = MAP_W;
    state->bmp.height = MAP_H;
    state->bmp.data = calloc(state->bmp.width * state->bmp.height,
                             sizeof(color_bgr_t));
    state->imageSize = bmp_calculate_size(&state->bmp);
    state->imageData = malloc(state->imageSize);
    state->run.x = MAP_W / 2.0;
    state->run.y = MAP_H / 2.0;
    state->userAction = 0;
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
    vector_t *runner = plot_tri(ROB_H, ROB_W, state->run.theta,
                                state->run.x, state->run.y, true);
    color_pixels(state, runner, red);

    bmp_serialize(&state->bmp, state->imageData);
    FILE *f = fopen("my_image.bmp", "wb");
    fwrite(state->imageData, state->imageSize, 1, f);
    fclose(f);
    image_server_set_data(state->imageSize, state->imageData);
    image_server_start("8000");
}

void runner_action(state_t *state) {
    switch (state->userAction) {
    case 1:
        state->run.v += 4.0;
        if (state->run.v > 12.0) {
            state->run.v = 12.0;
        }
        break;
    case 2:
        state->run.omega -= M_PI / 32.0;
        break;
    case 3:
        state->run.omega += M_PI / 32.0;
        break;
    default:
        break;
    }
}

void update_movement(state_t *state) {
    runner_action(state);
    rob_time_step(&state->run);
    if (fabs(state->run.omega) > 0.0) {
        printf("%.2f %.2f\n", state->run.v, -state->run.omega);
    } else {
        printf("%.2f %.2f\n", state->run.v, state->run.omega);
    }
    state->userAction = 0;
    state->timeStep++;
}

int main(int argc, char **argv) {
    bool disable = false;
    if (argc == 2 && !strcmp(argv[1], "disable")) {
        disable = true;
    }
    int seconds = 0;
    long nanoseconds = 40 * 1000 * 1000;
    struct timespec interval = {seconds, nanoseconds};
    state_t state = {0};
    state_init(&state);
    pthread_t thread;
    pthread_create(&thread, NULL, io_thread, &state);
    while (true) {
        update_movement(&state);
        if (!disable) {
            update_graphics(&state);
        }
        nanosleep(&interval, NULL);
    }
    free(state.bmp.data);
    free(state.imageData);

    return 0;
}
