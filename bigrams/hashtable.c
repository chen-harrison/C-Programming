#include "hashtable.h"

#define FACTOR 2654435769

typedef struct hashtable_entry {
    char *key;
    int value;
} hashtable_entry_t;

typedef struct hashtable {
    hashtable_entry_t *entries;
    int tableSize;
    int keys;
} hashtable_t;

hashtable_t *hashtable_create(void) {
    hashtable_t *ht = malloc(sizeof(hashtable_t));
    ht->tableSize = 128;
    ht->keys = 0;
    ht->entries = calloc(ht->tableSize, sizeof(hashtable_entry_t));
    return ht;
}

void hashtable_destroy(hashtable_t *ht) {
    for (int i = 0; i < ht->tableSize; i++) {
        free(ht->entries[i].key);
    }
    free(ht->entries);
    free(ht);
}

uint32_t rotate_left(uint32_t value, uint32_t count) {
    return value << count | value >> (32 - count);
}

uint32_t fxhash32_step(uint32_t hash, uint32_t value) {
    const uint32_t key = 0x27220a95;
    return (rotate_left(hash, 5) ^ value) * key;
}

uint32_t fxhash32_hash(char *data, int n) {
    uint32_t hash = 0;
    int size = 4;
    for (int i = 0; i < floor(n / 4.0); i++) {
        uint32_t number;
        memcpy(&number, data, size);
        hash = fxhash32_step(hash, number);
        data += size;
    }
    for (int i = 0; i < (n % 4); i++) {
        hash = fxhash32_step(hash, data[0]);
        data++;
    }
    return hash;
}

uint32_t fibonacci32_reduce(uint32_t hash, int bytes) {
    return (((uint32_t)(hash * FACTOR)) >> (32 - bytes));
}

void hashtable_set(hashtable_t *ht, char *key, int value) {
    double loadFactor = (double)(ht->keys) / (double)(ht->tableSize);
    if (loadFactor >= 0.5) {
        hashtable_expand(ht);
    }
    int n = (int)strlen(key);
    int bytes = (int)(log(ht->tableSize) / log(2));
    uint32_t hash = fibonacci32_reduce(fxhash32_hash(key, n), bytes);
    while (true) {
        if (!ht->entries[hash].key) {
            // n + 1 is to account for null char at end
            ht->entries[hash].key = malloc(n + 1);
            memcpy(ht->entries[hash].key, key, n + 1);
            ht->entries[hash].value = value;
            ht->keys++;
            break;
        }
        if (!strcmp(ht->entries[hash].key, key)) {
            ht->entries[hash].value += value;
            break;
        }
        hash = (hash + 1) % ht->tableSize;
    }
}

bool hashtable_get(hashtable_t *ht, char *key, int *value) {
    int n = (int)strlen(key);
    int bytes = (int)(log(ht->tableSize) / log(2));
    uint32_t hash = fibonacci32_reduce(fxhash32_hash(key, n), bytes);
    while (true) {
        if (!ht->entries[hash].key) {
            return false;
        }
        if (!strcmp(ht->entries[hash].key, key)) {
            *value = ht->entries[hash].value;
            return true;
        }
        hash = (hash + 1) % ht->tableSize;
    }
}

int hashtable_collisions(hashtable_t *ht) {
    int collisions = 0;
    int *test = calloc(ht->tableSize, sizeof(int));
    for (int i = 0; i < ht->tableSize; i++) {
        if (ht->entries[i].key) {
            char *key = ht->entries[i].key;
            int n = (int)strlen(key);
            int bytes = (int)(log(ht->tableSize) / log(2));
            uint32_t hash = fibonacci32_reduce(fxhash32_hash(key, n), bytes);
            if (!test[hash]) {
                test[hash] = 1;
            } else {
                collisions++;
            }
        }
    }
    free(test);
    return collisions;
}

void hashtable_expand(hashtable_t *ht) {
    int preCollisions = hashtable_collisions(ht);
    hashtable_t *big = malloc(sizeof(hashtable_t));
    big->tableSize = ht->tableSize * 2;
    big->keys = 0;
    big->entries = calloc(big->tableSize, sizeof(hashtable_entry_t));
    for (int i = 0; i < ht->tableSize; i++) {
        if (ht->entries[i].key) {
            char *key = ht->entries[i].key;
            int value = ht->entries[i].value;
            hashtable_set(big, key, value);
        }
        free(ht->entries[i].key);
    }
    free(ht->entries);

    ht->entries = big->entries;
    ht->tableSize = big->tableSize;
    ht->keys = big->keys;
    int postCollisions = hashtable_collisions(ht);
    printf("Rehashing reduced collisions from %d to %d\n",
           preCollisions, postCollisions);
    free(big);
}

int hashtable_size(hashtable_t *ht) {
    return ht->keys;
}

int hashtable_probe_max(hashtable_t *ht) {
    return ht->tableSize;
}

bool hashtable_probe(hashtable_t *ht, int i, char **key, int *val) {
    if (ht->entries[i].key) {
        *key = ht->entries[i].key;
        *val = ht->entries[i].value;
        return true;
    }
    return false;
}
