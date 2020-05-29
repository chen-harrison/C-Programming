#include "robot_t.h"

vector_t *rob_v(robot_t *rob) {
    double h = ROB_H / 2.0;
    double w = ROB_W / 2.0;
    pt_t hold[3] = {{-h, w}, {-h, -w}, {h, 0.0}};
    vector_t *vertices = vector_create();
    for (int i = 0; i < 3; i++) {
        vector_append(vertices, hold[i]);
    }
    rotate(vertices, rob->theta);
    translate(vertices, rob->x, rob->y);
    return vertices;
}

bool chase_collision(robot_t *chase, robot_t *run) {
    vector_t *chaseVertices = rob_v(chase);
    vector_t *runVertices = rob_v(run);
    bool out;
    if (intersect(chaseVertices, runVertices)) {
        out = true;
    } else {
        out = false;
    }
    free(chaseVertices->v);
    free(chaseVertices);
    free(runVertices->v);
    free(runVertices);
    return out;
}

vector_t *block_v(int x, int y) {
    pt_t hold[4] = {{0.0, 0.0}, {BLOCK_L, 0.0}, {BLOCK_L, BLOCK_L}, {0, BLOCK_L}};
    vector_t *vertices = vector_create();
    for (int i = 0; i < 4; i++) {
        vector_append(vertices, hold[i]);
    }
    translate(vertices, BLOCK_L * x, BLOCK_L * y);
    return vertices;
}

bool block_collision(robot_t *rob) {
    int x = floor(rob->x / BLOCK_L);
    int y = floor(rob->y / BLOCK_L);

    vector_t *robVertices = rob_v(rob);
    vector_t *blockVertices[9] = {0};
    int idx = 0;
    for (int j = y - 1; j <= y + 1; j++) {
        for (int i = x - 1; i <= x + 1; i++) {
            int pos = GRID_W * j + i;
            if ((pos >= 0 && pos < strlen(MAP)) && (MAP[pos] == 'X')) {
                blockVertices[idx] = block_v(i, j);
                idx++;
            }
        }
    }

    bool collided = false;
    bool loop = true;
    while (loop) {
        loop = false;
        for (int i = 0; i < idx; i++) {
            if (intersect(robVertices, blockVertices[i])) {
                collided = true;
                loop = true;
                pt_t blockCenter = vector_avg(blockVertices[i]);
                double pathLength = sqrt(pow(rob->x - blockCenter.x, 2) +
                                         pow(rob->y - blockCenter.y, 2));
                pt_t path = {(rob->x - blockCenter.x) / pathLength,
                             (rob->y - blockCenter.y) / pathLength};
                rob->x += path.x * 0.5;
                rob->y += path.y * 0.5;
                free(robVertices->v);
                free(robVertices);
                robVertices = rob_v(rob);
            }
        }
    }
    for (int i = 0; i < idx; i++) {
        free(blockVertices[i]->v);
        free(blockVertices[i]);
    }
    free(robVertices->v);
    free(robVertices);

    return collided;
}

void runner_action(robot_t *run, int action) {
    switch (action) {
    case 1:
        printf("%d ", action);
        run->v += 2;
        if (run->v > 12) {
            run->v = 12;
        }
        break;
    case 2:
        printf("%d ", action);
        run->omega -= M_PI / 16;
        break;
    default:
        printf("0 ");
        break;
    }
}

void chaser_action(robot_t *chase, int action) {
    switch (action) {
    case 1:
        chase->v += 2.0;
        if (chase->v > 12.0) {
            chase->v = 12.0;
        }
        break;
    case 2:
        chase->omega -= M_PI / 16;
        break;
    case 3:
        chase->omega += M_PI / 16;
        break;
    default:
        break;
    }
}

void rob_time_step(robot_t *rob) {
    rob->theta += rob->omega;
    rob->omega *= 0.8;
    rob->x += rob->v * cos(rob->theta);
    rob->y += rob->v * sin(rob->theta);
    if (block_collision(rob)) {
        rob->v *= 0.25;
    }
}

double chaser_sim(search_node_t node, int *chosenAction, int test) {
    chaser_action(&node.chase, test);
    // simulate 3 (?) timesteps
    for (int i = 0; i < 4; i++) {
        rob_time_step(&node.chase);
        if (chase_collision(&node.chase, &node.run)) {
            // if collided, return score of 0 and take corresponding action
            return 0.0;
        }
    }
    node.depth++;
    double score = search_actions(node, chosenAction);
    if (node.chase.v < 2.0) {
        score += 200.0 / node.chase.v;
    } else {
        score += 100.0;
    }

    return score;
}

double search_actions(search_node_t node, int *chosenAction) {
    if (node.depth >= MAX_DEPTH) {
        double dist = sqrt(pow(node.chase.x - node.run.x, 2) +
                           pow(node.chase.y - node.run.y, 2));
        return dist;
    }

    // chaser_sim calls search_actions with depth increased + 1, hence recursive
    // chosenAction holds the action one iteration down that yields the minimum of all 256 outcomes
    double scores[4] = {chaser_sim(node, chosenAction, 0),
                        chaser_sim(node, chosenAction, 1),
                        chaser_sim(node, chosenAction, 2),
                        chaser_sim(node, chosenAction, 3)};
    int minAction = 0;
    for (int i = 0; i < 4; i++) {
        if (scores[i] < scores[minAction]) {
            minAction = i;
        }
    }
    *chosenAction = minAction;
    return scores[minAction];
}
