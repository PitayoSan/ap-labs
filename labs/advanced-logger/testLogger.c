#include <stdio.h>
#include <stdlib.h>
#include "logger.h"

int main(int argc, char **argv){

    printf("%d\n", atoi(argv[1]));
    switch(atoi(argv[1])) {
		case 1:
			// default logging
			infof("INFO Message %d", 1);
			warnf("WARN Message %d", 1);
			errorf("ERROR Message %d", 1);
			break;
		case 2:
			// stdout logging
			initLogger("stdout");
			infof("INFO Message %d", 2);
			warnf("WARN Message %d", 2);
			errorf("ERROR Message %d", 2);
			break;
		case 3:
			// syslog logging
			initLogger("syslog");
			infof("INFO Message %d", 3);
			warnf("WARN Message %d", 3);
			errorf("ERROR Message %d", 3);
			break;
		default:
			errorf("Invalid test case");
			break;
    }

    return 0;
}
