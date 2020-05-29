
#include <stdint.h>
#include <ctype.h>
#include "hashtable.h"

bool read_word(FILE *f, char *wordBuffer) {
    char current;
    // searching for first letter in word
    while (fscanf(f, "%c", &current)) {
        if (feof(f)) {
            return false;
        }
        if (isalpha(current)) {
            wordBuffer[0] = current;
            wordBuffer++;
            break;
        }
    }
    // fills up the rest of the word, breaks when non-alpha found
    while (fscanf(f, "%c", &current)) {
        if (feof(f)) {
            return false;
        }
        if (isalpha(current)) {
            wordBuffer[0] = current;
            wordBuffer++;
        } else if (!isalpha(current)) {
            wordBuffer[0] = '\0';
            break;
        }
    }
    return true;
}

int main(void) {
    hashtable_t *ht = hashtable_create();
    FILE *f = fopen("book.txt", "r");
    char *word1 = malloc(256);
    read_word(f, word1);
    char *word2 = malloc(256);

    while (read_word(f, word2)) {
        int n1 = (int)strlen(word1);
        int n2 = (int)strlen(word2);
        word1[n1] = ' ';
        // copies word2 to pointer right after word1 + ' '
        memcpy(word1 + n1 + 1, word2, n2 + 1);
        // word1 now holds the full bigram, which is fed to the hashtable
        hashtable_set(ht, word1, 1);
        free(word1);
        word1 = malloc(256);
        // set word1 = word2 in preparation for next loop iter
        memcpy(word1, word2, n2 + 1);
        free(word2);
        word2 = malloc(256);
    }

    char *key[1] = {0};
    int val = 0;
    bool reach200 = false;
    for (int i = 0; i < hashtable_probe_max(ht); i++) {
        if (hashtable_probe(ht, i, key, &val)) {
            if (val >= 200) {
                printf("Bigram '%s' has count of %d\n", *key, val);
                reach200 = true;
            }
        }
    }
    if (!reach200) {
        for (int i = 0; i < hashtable_probe_max(ht); i++) {
            if (hashtable_probe(ht, i, key, &val)) {
                printf("Bigram '%s' has count of %d\n", *key, val);
            }
        }
    }
    printf("Total of %d different bigrams recorded\n", hashtable_size(ht));
    free(word1);
    free(word2);
    hashtable_destroy(ht);
    return 0;
}
