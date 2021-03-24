#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern int errno;

#define OUTPUT_PREFIX "sorted_"
#define MAX_LINE_LENGTH 33
#define USAGE "Usage:\n\
$ ./generic_merge_sort strings.txt\n\
$ ./generic_merge_sort strings.txt -o sorted_strings.txt\n\
$ ./generic_merge_sort -n numbers.txt\n\
$ ./generic_merge_sort -n numbers.txt -o sorted_numbers.txt\n"


int usage_error_exit() {
    printf(USAGE);
    return 0;
}

int compare_strings(void* s1, void* s2) {
    char *ms1 = *(char**)s1;
    char *ms2 = *(char**)s2;

    return strcmp(ms1, ms2);
}

int compare_ints(void* s1, void* s2) {
    int *i1 = (int*)s1;
    int *i2 = (int*)s2;

    if ((*i1) > (*i2)) {
        return 1;
    } else if ((*i1) < (*i2)) {
        return -1;
    } else {
        return 0;
    }
}

void swap(void* e1, void* e2, int size) {
    char buffer[size];

    memcpy(buffer, e1, size);
    memcpy(e1, e2, size);
    memcpy(e2, buffer, size);
}

void merge(void *arr, int size, int left, int midpoint, int right, int (*compare)(void*, void*)) {
    int const left_length = midpoint - left + 1;
    int const right_length = right - midpoint;

    void *left_array[left_length], *right_array[right_length];

    for (int index = 0; index < left_length; index++) {
        void *temp = (char*)(arr + (index * size));
        left_array[index] = temp;
    }

    for (int index = 0; index < right_length; index++) {
        void *temp = (char*)(arr + (index * size));
        right_array[index] = temp;
    }

    int left_subarray = 0;
    int right_subarray = 0;
    int merged_array = left;

    while (left_subarray < left_length && right_subarray < right_length) {
        void *temp;
        if (left_array[left_subarray] <= right_array[right_length]) {
            void *temp = (char*)(arr + (merged_array * size));
            swap(temp, left_array[left_subarray], size);
            left_subarray++;
        } else {
            void *temp = (char*)(arr + (merged_array * size));
            swap(temp, right_array[right_subarray], size);
            right_subarray++;
        }
        merged_array++;
    }

    while (left_subarray < left_length) {
        void *temp = (char*)(arr + (merged_array * size));
        swap(temp, left_array[left_subarray], size);
        left_subarray++;
        merged_array++;
    }

    while (right_subarray < right_length) {
        void *temp = (char*)(arr + (merged_array * size));
        swap(temp, right_array[right_subarray], size);
        right_subarray++;
        merged_array++;
    }
}


void merge_sort(void *arr, int size, int left, int right, int (*compare)(void*, void*)) {
    if (left < right) {
        int midpoint = (left + right)/2;
        merge_sort(arr, size, left, midpoint, compare);
        merge_sort(arr, size, midpoint + 1, right, compare);
        merge(arr, size, left, midpoint, right, compare);
    }
}


int main(int argc, char **argv) {
    char *input_name;
    char *output_name;
    int numeric = 0;

    switch (argc) {
    case 2:
        if (strcmp(argv[1], "-n") == 0) {
            return usage_error_exit();
        }
        input_name = argv[1];
        int output_name_length = strlen(input_name) + strlen(OUTPUT_PREFIX);
        output_name = (char*)calloc(output_name_length, sizeof(char));
        strcat(output_name, OUTPUT_PREFIX);
        strcat(output_name, input_name);
        break;

    case 3:
        if (strcmp(argv[1], "-n") == 0) {
            numeric = 1;
            input_name = argv[2];
            int output_name_length = strlen(input_name) + strlen(OUTPUT_PREFIX);
            output_name = (char*)calloc(output_name_length, sizeof(char));
            strcat(output_name, OUTPUT_PREFIX);
            strcat(output_name, input_name);
            break;
        }
        return usage_error_exit();

    case 4:
        if (strcmp(argv[2], "-o") == 0) {
            input_name = argv[1];
            output_name = argv[3];
            break;
        }
        return usage_error_exit();

    case 5:
        if (strcmp(argv[1], "-n") == 0 && strcmp(argv[3], "-o") == 0) {
            numeric = 1;
            input_name = argv[2];
            output_name = argv[4];
            break;
        }
        return usage_error_exit();
    
    default:
        return usage_error_exit();
    }

    FILE *fpw;
    int return_code = 0;

    FILE *fpr = fopen(input_name, "r");
    if (fpr == NULL) {
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error opening file");
        return 0;
    }

    int nlines = 0;
        char current_line[MAX_LINE_LENGTH];

    int fgetcode = 0;
    while(!feof(fpr)) {
        fgetcode = fgetc(fpr);
        if(fgetcode == '\n') {
            nlines++;
        }
    }

    rewind(fpr);

    char *string_lines[nlines];
    int line_counter = 0;
    int ncurrent_line = 0;

    if (numeric) {
        while (fgets(current_line, MAX_LINE_LENGTH, (FILE*)fpr) != NULL) {
            string_lines[ncurrent_line] = (char*)calloc(MAX_LINE_LENGTH, sizeof(char));
            strcpy(string_lines[ncurrent_line], current_line);
            ncurrent_line++;
        }

        int number_lines[nlines];
        for (int index = 0; index < nlines; index++) {
            number_lines[index] = atoi(string_lines[index]);
        }   
        int *np = number_lines;

        merge_sort(np, sizeof(int), 0, nlines, (int (*)(void*, void*))(compare_ints));

        fpw = fopen(output_name, "w");
        if (fpw == NULL) {
            fprintf(stderr, "Value of errno: %d\n", errno);
            perror("Error opening file");
            return 0;
        }

        for (int index = 0; index < nlines; index++) {
            return_code = fprintf(fpw, "%d\n", number_lines[index]);
        }
    } else {
        while (fgets(current_line, MAX_LINE_LENGTH, (FILE*)fpr) != NULL) {
            if (line_counter % 2 == 0) {
                string_lines[ncurrent_line] = (char*)calloc(MAX_LINE_LENGTH, sizeof(char));
                strcpy(string_lines[ncurrent_line], current_line);
                ncurrent_line++;
            }
            line_counter++;
        }
 
        merge_sort(string_lines, sizeof(char*), 0, nlines, (int (*)(void*, void*))(compare_strings));

        fpw = fopen(output_name, "w");
        if (fpw == NULL) {
            fprintf(stderr, "Value of errno: %d\n", errno);
            perror("Error opening file");
            return 0;
        }

        return_code = fwrite(string_lines, sizeof(char), sizeof(string_lines), fpw);
    }

    if (return_code < 0) {
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error writing to file");
        return 0;
    }
    fclose(fpr);
    fclose(fpw);

    printf("Results file can be found at %s\n", output_name);
    
    return 0;
}
