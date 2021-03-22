int mystrlen(char *str){
    int length = 0;

    while (str[length] != '\0') {
        length++;
    }

    return length;
}

char *mystradd(char *origin, char *addition){
    int origin_length = mystrlen(origin);

    for (int index = 0; addition[index] != '\0'; index++, origin_length++) {
        origin[origin_length] = addition[index];
    }
}

int mystrfind(char *origin, char *substr){
    for (int index1 = 0; origin[index1] != '\0'; index1++) {
        int substr_length = mystrlen(substr);
        for (int index2 = 0; substr[index2] != '\0'; index2++) {
            if (origin[index1 + index2] != substr[index2]) {
                break;
            }
            if (index2 + 1 == substr_length) {
                return index1;
            }
        }
    }
    return -1;
}
