#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define WORD_MAX_LEN 256

typedef struct tst_node {
    char c;
    struct tst_node *low;
    struct tst_node *equal;
    struct tst_node *high;
} tst_node_t;

typedef struct tst {
    tst_node_t *node;
} tst_t;

tst_node_t *tst_node_create(char c) {
    tst_node_t *node = calloc(1, sizeof(*node));
    node->c = c;
    return node;
}

void tst_node_destroy(tst_node_t *node) {
    if (node->low) {
        tst_node_destroy(node->low);
    }
    if (node->equal) {
        tst_node_destroy(node->equal);
    }
    if (node->high) {
        tst_node_destroy(node->high);
    }
    free(node);
}

tst_t *tst_create(void) {
    return calloc(1, sizeof(tst_t));
}

void tst_destroy(tst_t *tst) {
    tst_node_destroy(tst->node);
    free(tst);
}

void tst_add(tst_t *tst, const char *word) {
    if (!tst->node) {
        tst->node = tst_node_create(word[0]);
    }
    int i = 0;
    tst_node_t *current = tst->node;
    while (true) {
        if (word[i] < current->c) {
            if (!current->low) {
                current->low = tst_node_create(word[i]);
            }
            current = current->low;
        } else if (word[i] > current->c) {
            if (!current->high) {
                current->high = tst_node_create(word[i]);
            }
            current = current->high;
        } else if (word[i] == current->c) {
            if (word[i] == '\0') {
                return;
            }
            if (!current->equal) {
                current->equal = tst_node_create(word[i + 1]);
            }
            current = current->equal;
            i++;
        }
    }
}

void tst_node_search(tst_node_t *node, char *word, char *suggestion, char *suggStart, int errs) {
    while (node) {
        if (errs > 0) {
            // insertion
            tst_node_search(node, word + 1, suggestion, suggStart, errs - 1);
            // deletion
            suggestion[0] = node->c;
            tst_node_search(node->equal, word, suggestion + 1, suggStart, errs - 1);
            // replacement
            suggestion[0] = node->c;
            tst_node_search(node->equal, word + 1, suggestion + 1, suggStart, errs - 1);
            // transposition
            char trans[sizeof(word) + 4];
            strcpy(trans, word);
            char swap = trans[0];
            trans[0] = trans[1];
            trans[1] = swap;
            tst_node_search(node, trans, suggestion, suggStart, errs - 1);
        }
        if (word[0] < node->c) {
            if (errs > 0) {
                tst_node_search(node->high, word, suggestion, suggStart, errs);
            }
            node = node->low;
        } else if (word[0] > node->c) {
            if (errs > 0) {
                tst_node_search(node->low, word, suggestion, suggStart, errs);
            }
            node = node->high;
        } else if (word[0] == node->c) {
            if (errs > 0) {
                tst_node_search(node->high, word, suggestion, suggStart, errs);
                tst_node_search(node->low, word, suggestion, suggStart, errs);
            }
            suggestion[0] = node->c;
            if (word[0] == '\0') {
                printf("%s\n", suggStart);
            }
            node = node->equal;
            word++;
            suggestion++;
        }
    }
}

void tst_search(tst_t *tst, char *word) {
    if (!tst->node) {
        return;
    }
    char wordSuggestion[WORD_MAX_LEN];
    tst_node_search(tst->node, word, &wordSuggestion[0], wordSuggestion, 1);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <word>+\n", argv[0]);
        return 1;
    }
    FILE *f = fopen("words.txt", "r");
    if (!f) {
        perror("Failed to open words.txt");
        return 1;
    }
    tst_t *tst = tst_create();
    char strBuffer[WORD_MAX_LEN];
    while (1) {
        int ret = fscanf(f, "%255s", strBuffer);
        if (ret != 1) {
            break;
        }
        tst_add(tst, strBuffer);
    }
    fclose(f);
    for (int i = 1; i < argc; i++) {
        if (strlen(argv[i]) >= WORD_MAX_LEN) {
            fprintf(stderr, "word '%s' is too long!\n", argv[i]);
            continue;
        }

        tst_search(tst, argv[i]);
        printf("\n");
    }

    tst_destroy(tst);
    return 0;
}
