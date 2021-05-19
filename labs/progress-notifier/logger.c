#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include "logger.h"


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


int out = 0;
// 0: STDOUT
// 1: SYSLOG

int initLogger(char *logType) {
    printf("Initializing Logger on: %s\n", logType);

    // Check for logger type
    if(strcmp(logType, "stdout") == 0) {
        ;
    } else if(strcmp(logType, "syslog") == 0) {
        out = 1;
    } else {
        printf("Invalid input\n");
        return -1;
    }

    // Create child process (daemon)
    switch(fork()) {
        // Error present
        case -1:
            return -1;
        // Child created
        case 0:
            break;
        // Killing parent
        default:
            exit(0);
    }

    return 0;
}

void textcolor(int attr, int fg, int bg) {
    char command[13];
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	printf("%s", command);
}

int closeLogger() {
    exit(0);
    return 0;
}

int infof(const char *format, ...) {
    va_list ap;
    va_start(ap, format);

    if(!out) {
        vfprintf(stdout, format, ap);
    } else {
        openlog("[INFO]", LOG_CONS, LOG_SYSLOG);
        vsyslog(LOG_INFO, format, ap);
        closelog();
    }

    va_end(ap);
    return 0;
}

int warnf(const char *format, ...) {
    va_list ap;
    va_start(ap, format);

    if(!out) {
        textcolor(BRIGHT, WHITE, YELLOW);
        printf("[WARNING]");
        textcolor(RESET, WHITE, BLACK);
        printf(": ");

        vfprintf(stdout, format, ap);
    } else {
        openlog("[WARNING]", LOG_CONS, LOG_SYSLOG);
        vsyslog(LOG_WARNING, format, ap);
        closelog();
    }

    va_end(ap);
    return 0;
}

int errorf(const char *format, ...) {
    va_list ap;
    va_start(ap, format);

    if(!out) {
        textcolor(BRIGHT, BLACK, RED);
        printf("[ERROR]: ");

        vfprintf(stdout, format, ap);

        textcolor(RESET, WHITE, BLACK);
    } else {
        openlog("[ERROR]", LOG_CONS, LOG_SYSLOG);
        vsyslog(LOG_ERR, format, ap);
        closelog();
    }

    va_end(ap);
    return 0;
}

int panicf(const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    
    if(!out) {
        textcolor(BRIGHT, WHITE, GREEN);
        printf("[PANIC]: ");

        vfprintf(stdout, format, ap);

        textcolor(RESET, WHITE, BLACK);
    } else {
        openlog("[PANIC]", LOG_CONS, LOG_SYSLOG);
        vsyslog(LOG_EMERG, format, ap);
        closelog();
    }

    va_end(ap);
    return 0;
} 
