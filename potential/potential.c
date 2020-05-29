#define _GNU_SOURCE

#include <unistd.h>
#include <time.h>
#include <string.h>
#include <termios.h>
#include <pthread.h>
#include "state_t.h"

#define NUM_PARAMETERS 7

struct termios originalTermios;
void reset_terminal(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
}

void *io_thread(void *user) {
    state_t *state = user;
    printf("\e[?25l");
    print_interface(state);

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
            printf("\e[?25h");
            printf("\n");
            free(state->bmp.data);
            free(state->imageData);
            exit(0);
        } else if (c == 'r') {
            state_reset(state);
        } else if (c == 27) {
            if (getc(stdin) == '[') {
                switch (getc(stdin)) {
                case 'A':
                    // UP
                    adjust_parameters(state, 1);
                    break;
                case 'B':
                    // DOWN
                    adjust_parameters(state, -1);
                    break;
                case 'D':
                    // LEFT
                    if (state->key == 1) {
                        state->key = NUM_PARAMETERS;
                    } else {
                        state->key--;
                    }
                    break;
                case 'C':
                    // RIGHT
                    if (state->key == NUM_PARAMETERS) {
                        state->key = 1;
                    } else {
                        state->key++;
                    }
                    break;
                default:
                    break;
                }
            }
        }
        print_interface(state);
    }
}

int main(int argc, char **argv) {
    state_t state = {0};
    state_init(&state);
    if (argc == 2 && !strcmp(argv[1], "disable")) {
        state.disable = true;
    }

    int seconds = 0;
    long nanoseconds = 40000 * 1000;
    struct timespec interval = {seconds, nanoseconds};
    int delay_count = 0;
    pthread_t thread;
    pthread_create(&thread, NULL, io_thread, &state);

    while (true) {
        update_movement(&state);
        delay_count++;

        if (delay_count == state.delayEvery) {
            if (!state.disable) {
                update_graphics(&state);
                nanosleep(&interval, NULL);
            }
            delay_count = 0;
        }
        if (state.runnerCaught) {
            if (!state.disable) {
                update_graphics(&state);
                nanosleep(&interval, NULL);
            }
            printf("\e[2K\rRunner caught on step %d\n", state.time_step - 1);
            print_interface(&state);
            state_reset(&state);
            delay_count = 0;
        }
    }
    free(state.bmp.data);
    free(state.imageData);
    
    return 0;
}
