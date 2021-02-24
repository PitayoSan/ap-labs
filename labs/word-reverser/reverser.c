#include <stdio.h>
#define ARR_LENGTH 100

int main() {
    int i = 0;
    char c, word[ARR_LENGTH], reversed[ARR_LENGTH];

    while ((c = getchar()) != EOF) {
        if (c == '\n') {
            for (int j = 0; j < i; j++) {
                reversed[j] = word[i - j - 1];
            }
            printf("%s\n", reversed);
            for (int j = 0; j < i; j++) {
                word[j] = 0;
                reversed[j] = 0;
            }
            i = 0;
        } else {
            word[i] = c;
            i++;
        }
    }
    return 0;
}