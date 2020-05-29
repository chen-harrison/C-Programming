#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPH_LENGTH 26
#define UPPER_TO_LOWER 32

int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "usage: ./cryptogram <encrypt|decrypt> <password> <text>\n");
        return 1;
    }
    char *command = argv[1];
    char *passRaw = argv[2];
    char *text = argv[3];
    char passNew[strlen(passRaw)];

    // uppercase to lowercase, omit non-alpha characters
    int j = 0;
    for (int i = 0; i < strlen(passRaw); i++) {
        if (isalpha(passRaw[i])) {
            if (passRaw[i] <= 'Z') {
                passRaw[i] += UPPER_TO_LOWER;
            }
            passNew[j] = (char)(passRaw[i] - 'a');
            j++;
        }
    }
    if (j == 0) {
        passNew[0] = '\0';
    }
    int passLength = j;
    j = 0;

    if (strcmp(command, "encrypt") == 0) {
        for (int i = 0; i < strlen(text); i++) {
            if (isalpha(text[i])) {
                if ((text[i] <= 'Z' && text[i] + passNew[j] > 'Z') ||
                    (text[i] >= 'a' && text[i] + passNew[j] > 'z')) {
                    text[i] -= ALPH_LENGTH;
                }
                text[i] = (char)(text[i] + passNew[j]);
                j++;
                if (j >= passLength) {
                    j = 0;
                }
            }
        }
    } else if (strcmp(command, "decrypt") == 0) {
        for (int i = 0; i < strlen(text); i++) {
            if (isalpha(text[i])) {
                if ((text[i] <= 'Z' && text[i] - passNew[j] < 'A') ||
                    (text[i] >= 'a' && text[i] - passNew[j] < 'a')) {
                    text[i] += ALPH_LENGTH;
                }
                text[i] = (char)(text[i] - passNew[j]);
                j++;
                if (j >= passLength) {
                    j = 0;
                }
            }
        }
    } else {
        fprintf(stderr, "expected command encrypt or decrypt. got '%s'\n", command);
        return 1;
    }

    printf("%s\n", text);
    return 0;
}
