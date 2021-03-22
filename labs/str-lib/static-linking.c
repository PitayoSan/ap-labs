#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int mystrlen(char *str);
char *mystradd(char *origin, char *addition);
int mystrfind(char *origin, char *substr);

int main(int argc, char **argv) {
    if (argc < 4 || !(strcmp(argv[1], "-add") == 0 || strcmp(argv[1], "-find") == 0)) {
        printf("Usage:\n");
        printf("$ ./main.o -add \"Initial String \" \"This is the rest to be added\"\n");
        printf("$ ./main.o -find \"Initial String \" \"Init\"\n");
    } else if (strcmp(argv[1], "-add") == 0) {
        int initial_length = mystrlen(argv[2]);
        int addition_length = mystrlen(argv[3]);
        int const new_string_length = initial_length + addition_length ;

        char *new_string = (char*)calloc(new_string_length, sizeof(char));
        mystradd(new_string, argv[2]);
        mystradd(new_string, argv[3]);

        int new_length = mystrlen(new_string);

        printf("Initial Length    : %d\n", initial_length);
        printf("New String        : %s\n", new_string);
        printf("New Length        : %d\n", new_length);
    } else if (strcmp(argv[1], "-find") == 0) {
        if (mystrlen(argv[3]) > mystrlen(argv[2])) {
            printf("[%s] is longer than [%s]. Cannot compute search\n", argv[3], argv[2]);
            return 0;
        } else if(mystrlen(argv[2]) == 0 || mystrlen(argv[3]) == 0) {
            printf("Cannot compute search with empty strings\n");
            return 0;
        }

        int position = mystrfind(argv[2], argv[3]);

        if (position >= 0) {
            printf("[%s] string was found at [%d] position\n", argv[3], position);
        } else {
            printf("[%s] string was not found\n", argv[3]);
        }
    }
    return 0;
}
