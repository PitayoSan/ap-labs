#define _XOPEN_SOURCE 500
#include <stdio.h>
#include "logger.h"
#include <sys/inotify.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <stdint.h>


#define NAME_MAX_LEN 32
#define BUFFER_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX_LEN + 1))
#define MAX_DIR_LEVEL 2
#define MIN_CHARS_IN_DISPLAY 30


static int list_counter = 0;
static char **file_list = NULL;
static int list_len = 0;
static const int list_increment = 1000;


static int display_event(struct inotify_event *event) {
    int string_format_code = 0;
    char *event_string = (char*)calloc(MIN_CHARS_IN_DISPLAY + (event->len), sizeof(char));

    if((event->mask & IN_CREATE) || (event->mask & IN_CREATE) || (event->mask & IN_DELETE) || (event->mask & IN_MOVED_FROM)) {
        if(event->mask & IN_ISDIR) {
            strcat(event_string, "- [Directory - ");
        } else {
            strcat(event_string, "- [File - ");
        }

        if(event->mask & IN_CREATE) {
            strcat(event_string, "Create] - ");
            strcat(event_string, event->name);
            strcat(event_string, "\n");
        } else if(event->mask & IN_DELETE) {
            strcat(event_string, "Removal] - ");
            strcat(event_string, event->name);
            strcat(event_string, "\n");
        } else if(event->mask & IN_MOVED_FROM) {
            strcat(event_string, "Rename] - ");
            strcat(event_string, event->name);
        }

    } else if(event->mask & IN_MOVED_TO) {
        strcat(event_string, " -> ");
        strcat(event_string, event->name);
        strcat(event_string, "\n");
    }

    infof(event_string);

    return 0;
}


static int display_info(const char *fpath, const struct stat *stat, int tflag, struct FTW *ftwbuf) {
    if(ftwbuf->level > MAX_DIR_LEVEL) {
        return 0;
    }

    const char *name = fpath;

    if (list_counter >= list_len) {
        list_len += list_increment;
        file_list = realloc(file_list, list_len * sizeof(char *));
        if (file_list == NULL) {
            errorf("Failure during alloc\n");
            return 1;
        }
    }

    file_list[list_counter] = strdup(name);
    list_counter++;

    return 0;
}


int main(int argc, char **argv){
    // Place your magic here

    if(argc != 2) {
        infof("Usage:\n$ ./main.o <directory>\n");
        return 0;
    }

    infof("Starting File/Directory Monitor on %s\n", argv[1]);
    infof("-----------------------------------------------------\n");
    int inotifyFd, working_directory;
    inotifyFd = inotify_init();
    if(inotifyFd == -1) {
        errorf("Failure on inotify init\n");
        return 1;
    }   

    struct inotify_event *event;
    char buffer[BUFFER_LEN] __attribute__ ((aligned(8)));
    ssize_t n_read;
    char* temp;

    int flags = 0;
    flags |= FTW_DEPTH;
    flags |= FTW_PHYS;

    if(nftw(argv[1], display_info, 20, flags) != 0) {
        errorf("Failure in nftw\n");
        return 1;
    }

    for(int counter = 0; counter < list_counter; counter++) {
        working_directory = inotify_add_watch(inotifyFd, file_list[counter], IN_ALL_EVENTS);
        if(working_directory == -1) {
            errorf("Failure on inotify watch\n");
            return 1;
        }
    }

    for(;;) {
        n_read = read(inotifyFd, buffer, BUFFER_LEN);
        if(n_read == 0) {
            panicf("read inotify returned 0\n");
            return 1;
        } else if(n_read == -1) {
            errorf("Failure on inotify watch\n");
            return 1;
        }

        for(temp = buffer; temp < buffer + n_read;) {
            event = (struct inotify_event *) temp;
            int display_code = display_event(event);
            if(display_code == 1) {
                errorf("Failure in display of event\n");
                return 1;
            }
            temp += sizeof(struct inotify_event) + event -> len;
        }
    }

    return 0;
}
