#include <stdio.h>
#include <stdarg.h>


/*
Credits to Pradeep Padala of https://www.linuxjournal.com/article/8603
for the control and color codes,
as well as the textcolor() function,
*/

#define RESET		0
#define BRIGHT 		1
#define DIM		    2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

#define BLACK 		0
#define RED		    1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7


int infof(const char *format, ...);
int warnf(const char *format, ...);
int errorf(const char *format, ...);
int panicf(const char *format, ...);


void textcolor(int attr, int fg, int bg) {
    char command[13];
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	printf("%s", command);
}

int infof(const char *format, ...) {
    textcolor(BRIGHT, BLACK, WHITE);
    printf("[INFO]");
    textcolor(RESET, WHITE, BLACK);
    printf(" - ");

    va_list ap;
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);

    printf("\n");
    return 0;
}

int warnf(const char *format, ...) {
    textcolor(BRIGHT, WHITE, YELLOW);
    printf("[WARNING]");
    textcolor(RESET, WHITE, BLACK);
    printf(" - ");

    va_list ap;
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);

    printf("\n");
    return 0;
}

int errorf(const char *format, ...) {
    textcolor(BRIGHT, BLACK, RED);
    printf("[ERROR] - ");

    va_list ap;
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);

    textcolor(RESET, WHITE, BLACK);

    printf("\n");
    return 0;
}

int panicf(const char *format, ...) {
    textcolor(BRIGHT, WHITE, GREEN);
    printf("[PANIC] - ");

    va_list ap;
    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);

    textcolor(RESET, WHITE, BLACK);

    printf("\n");
    return 0;
}