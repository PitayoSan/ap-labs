#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#define REPORT_FILE "packages_report.txt"
#define MAX_PACKAGES 600
#define MAX_REPORT_FILE_SIZE 100

// Carlos Ernesto López Solano
// A01633683

extern int errno;


// Struct definition
struct Package {
    char *name;
    time_t installDate;
    int installCount;
    time_t lastUpdateDate;
    int updateCount;
    time_t removalDate;
    int removalCount;
};


// Struct array global declaration
struct Package **Packages;


// General Stats
int alpmscCount = 0;
int alpmCount = 0;
int pacmanCount = 0;


// Package Count
int installed = 0;
int removed = 0;
int upgraded = 0;


// Utilities
void getMaximumLineLength(FILE *fp, int *maxLength) {
    int currentCharacter, currentLine = 0;

    for (;;) {
        currentCharacter = fgetc(fp);
        if (currentCharacter == '\n') {
            if (currentLine > *maxLength) {
                *maxLength = currentLine + 2;
            }
            currentLine = 0;
        } else if (currentCharacter == EOF) {
            break;
        }
        currentLine++;
    }
}


int packageIsRegistered(char *name, struct Package **Packages) {
    for (int index = 0; index < MAX_PACKAGES; index++) {
        if (Packages[index] == NULL) break;

        if (strcmp(Packages[index]->name, name) == 0) {
            return index;
        }
    }
    return -1;
}


int insertPackage(struct Package *NewPackage) {
    for (int index = 0; index < MAX_PACKAGES; index++) {
        if (Packages[index] == NULL) {
            Packages[index] = NewPackage;
            return index;
        }
    }
    perror("Packages buffer is full");
    return -1;
}


void parseDate(char *rawDate, time_t *processedDate, struct tm *parsedDate) {
    int year, mon, mday, hour, min;

    if (strstr(rawDate, "T") == NULL) {
        sscanf(rawDate, "%d-%d-%d %d:%d", &year, &mon, &mday, &hour, &min);
    } else {
        sscanf(rawDate, "%d-%d-%dT%d:%d", &year, &mon, &mday, &hour, &min);
    }
    
    // Substractions obtained from:
    // https://stackoverflow.com/questions/23693669/mktime-returns-wrong-timestamp-off-by-a-whole-month
    parsedDate->tm_year = year - 1900; // years since 1900
    parsedDate->tm_mon = mon - 1; // months since January — [0, 11]
    parsedDate->tm_mday = mday; // day of the month — [1, 31]
    parsedDate->tm_hour = hour - 1; // hours since midnight — [0, 23]
    parsedDate->tm_min = min; // minutes after the hour — [0, 59]
    *processedDate = mktime(parsedDate);
}


// Error handling funtions
int checkFileOpenedCorrectly(FILE *fp) {
    if (fp == NULL) {
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error opening file");
        return 1;
    }
    return 0;
}


int checkWriteWasSuccesful(int returnCode) {
    if (returnCode < 0) {
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error writing to file");
        return 1;
    }
    return 0;
}


int checkAllocationWasSuccessful(char *pointer) {
    if (pointer == NULL) {
        perror("Failure during memory allocation");
        return 1;
    }
    return 0;
}


int checkStringFormatWasSuccessful(int returnCode) {
    if (returnCode < 0) {
        perror("Failure during memory allocation");
        return 1;
    }
    return 0;
}


// Log analysis
int parseLine(char *line) {
    int index = 0;
    int dateCounter = index + 1;
    char currentChar = line[dateCounter];

    // Get date
    char *date = (char*) calloc(30, sizeof(char));
    if(checkAllocationWasSuccessful(date) == 1) return 1;
    while (currentChar != ']') {
        date[index] = currentChar;
        index++;
        dateCounter++;
        currentChar = line[dateCounter];
    }
    index += 4;
    currentChar = line[index];

    // Get type
    int typeCounter = 0;
    char *type = (char*) calloc(20, sizeof(char));
    if(checkAllocationWasSuccessful(type) == 1) return 1;
    while (currentChar != ']') {
        type[typeCounter] = currentChar;
        index++;
        typeCounter++;
        currentChar = line[index];
    }

    if (strcmp(type, "PACMAN") == 0) {
        pacmanCount++;
    } else if (strcmp(type, "ALPM") == 0) {
        alpmCount++;
    } else if (strcmp(type, "ALPM-SCRIPTLET") == 0) {
        alpmscCount++;
    }

    // Get operation
    index += 2;
    currentChar = line[index];
    int operationCounter = 0;
    char *operation = (char*) calloc(20, sizeof(char));
    if(checkAllocationWasSuccessful(operation) == 1) return 1;
    while (currentChar != ' ') {
        operation[operationCounter] = currentChar;
        index++;
        operationCounter++;
        currentChar = line[index];
    }

    if (strcmp(operation, "installed") == 0 || strcmp(operation, "upgraded") == 0 || strcmp(operation, "removed") == 0 ) {
        // Get name
        index++;
        currentChar = line[index];
        int nameCounter = 0;
        char *name = (char*) calloc(50, sizeof(char));
        if(checkAllocationWasSuccessful(name) == 1) return 1;
        while (currentChar != ' ') {
            name[nameCounter] = currentChar;
            index++;
            nameCounter++;
            currentChar = line[index];
        }
        
        int structIndex = packageIsRegistered(name, Packages);
        if (structIndex < 0) {
            struct Package *Package1 = calloc(1, sizeof(struct Package));
            Package1->name = name;
            Package1->installDate = (time_t)calloc(1, sizeof(time_t));
            Package1->lastUpdateDate = (time_t)calloc(1, sizeof(time_t));
            Package1->removalDate = (time_t)calloc(1, sizeof(time_t));

            structIndex = insertPackage(Package1);
            if(structIndex == -1) return 1;
        }
        
        struct Package *PackageUsed = Packages[structIndex];
        
        time_t processedDate = (time_t)calloc(1, sizeof(time_t));
        struct tm *parsedDate = (struct tm*)calloc(1, sizeof(struct tm));
        parseDate(date, &processedDate, parsedDate);

        if (strcmp(operation, "installed") == 0) {
            installed += 1;
            PackageUsed->installDate = processedDate;
            PackageUsed->installCount++;
        } else if (strcmp(operation, "upgraded") == 0) {
            upgraded += 1;
            PackageUsed->lastUpdateDate = processedDate;
            PackageUsed->updateCount++;
        } else if (strcmp(operation, "removed") == 0) {
            removed += 1;
            PackageUsed->removalDate = processedDate;
            PackageUsed->removalCount++;
        }
    }
    return 0;
}


int readLog(FILE *fp, int maxLineLength) {
    char currentLine[maxLineLength];
    while (fgets(currentLine, maxLineLength, (FILE*)fp) != NULL) {
        if (strstr(currentLine, "PACMAN") != NULL || strstr(currentLine, "ALPM") != NULL || strstr(currentLine, "ALPM-SCRIPTLET") != NULL) {
            if(parseLine(currentLine) == 1) return 1;
        }   
    }
    return 0;
}


int analizeLog(char *logFile) {
    printf("Generating Report from: [%s] log file\n", logFile);

    FILE *fp = fopen(logFile, "r");
    if(checkFileOpenedCorrectly(fp) == 1) return 1;

    int maxLineLength = 0;
    getMaximumLineLength(fp, &maxLineLength);

    rewind(fp);

    if(readLog(fp, maxLineLength) == 1) return 1;

    fclose(fp);
    
    return 0;
}


// Report creation
int generateReport(char *report) {
    if (Packages[0] == NULL) {
        printf("No packages to report\n");
        return 0;
    }

    printf("Report is generated at: [%s]\n", report);

    int stringFormatCode = 0;
    FILE *fp = fopen(report, "w");
    if(checkFileOpenedCorrectly(fp) == 1) return 1;

    // Headers
    char *headers = (char*)calloc(250, sizeof(char));
    stringFormatCode = sprintf(
        headers,
        "Pacman Packages Report\n"
        "----------------------\n"
        "- Installed packages          : %d\n"
        "- Removed Packages            : %d\n"
        "- Upgraded packages           : %d\n"
        "- Current installed           : %d\n",
        installed,
        removed,
        upgraded,
        installed - removed
    );

    if(checkStringFormatWasSuccessful(stringFormatCode) == 1) return 1;
    if(checkWriteWasSuccesful(fputs(headers, fp)) == 1) return 1;

    free(headers);

    struct Package *reference = Packages[0];
    char *oldestPackage = (char*)calloc(26, sizeof(char));
    char *newestPackage = (char*)calloc(26, sizeof(char));
    time_t oldestPackageDate = (time_t)calloc(1, sizeof(time_t));
    time_t newestPackageDate = (time_t)calloc(1, sizeof(time_t));
    char noUpgrades[4000];

    oldestPackage = reference->name;
    newestPackage = reference->name;
    oldestPackageDate = reference->installDate;
    newestPackageDate = reference->installDate;

    for (int index = 0; index < MAX_PACKAGES; index++) {
        if (Packages[index] == NULL) break;
        
        struct Package *currentPackage = Packages[index];
        if (currentPackage->updateCount == 0) {
            strcat(noUpgrades, currentPackage->name);
            if (Packages[index + 1] != NULL) strcat(noUpgrades, ", ");
        }

        if (currentPackage->installCount > 0) {
            double oldDifference = difftime(oldestPackageDate, currentPackage->installDate);
            if (oldDifference > 0) {
                oldestPackage = currentPackage->name;
                oldestPackageDate = currentPackage->installDate;
            }

            double newDifference = difftime(newestPackageDate, currentPackage->installDate);
            if (newDifference <= 0) {
                newestPackage = currentPackage->name;
                newestPackageDate = currentPackage->installDate;
            }
        }
    }

    // General Stats
    char *generalStats = (char*)calloc(4000, sizeof(char));
    stringFormatCode = sprintf(
        generalStats, 
        "-------------\n"
        "General Stats\n"
        "-------------\n"
        "- Oldest package              : %s\n"
        "- Newest package              : %s\n"
        "- Packages with no upgrades   : %s\n"
        "- [ALPM-SCRIPTLET] type count : %d\n"
        "- [ALPM] count                : %d\n"
        "- [PACMAN] count              : %d\n",
        oldestPackage,
        newestPackage,
        noUpgrades,
        alpmscCount,
        alpmCount,
        pacmanCount
    );

    if(checkStringFormatWasSuccessful(stringFormatCode) == 1) return 1;
    if(checkWriteWasSuccesful(fputs(generalStats, fp)) == 1) return 1;
    
    free(generalStats);
    free(oldestPackage);
    free(newestPackage);

    // List of packages
    char *packagesList =
    "----------------\n"
    "List of packages\n"
    "----------------\n";

    if(checkWriteWasSuccesful(fputs(packagesList, fp)) == 1) return 1;

    for (int index = 0; index < MAX_PACKAGES; index++) {
        if (Packages[index] == NULL) break;
        
        struct Package *currentPackage = Packages[index];

        char *currentStats = malloc(250 * sizeof(char));
        stringFormatCode = sprintf(
            currentStats,
            "- Package Name                : %s\n"
            "  - Install date              : %s"
            "  - Last update date          : %s"
            "  - How many updates          : %d\n"
            "  - Removal date              : %s",
            currentPackage->name,
            currentPackage->installCount == 0 ? "-\n" : ctime(&currentPackage->installDate),
            currentPackage->updateCount == 0 ? "-\n" : ctime(&currentPackage->lastUpdateDate),
            currentPackage->updateCount,
            currentPackage->removalCount == 0 ? "-\n" : ctime(&currentPackage->removalDate)

        );
        if(checkStringFormatWasSuccessful(stringFormatCode) == 1) return 1;
        if(checkWriteWasSuccesful(fputs(currentStats, fp)) == 1) return 1;

        free(currentStats);
    }

    fclose(fp);
    return 0;
}


int main(int argc, char **argv) {

    if (argc < 3 || strcmp(argv[1], "-input") != 0) {
	    printf("Usage: $ ./pacman-analizer.o -input <input>.txt -report <report>.txt\n");
	    return 1;
    }

    char *report_file = (char*)calloc(MAX_REPORT_FILE_SIZE, sizeof(char));
    if (argc < 5 && argv[4] != "-report") {
        report_file = REPORT_FILE;
    } else {
        report_file = argv[4];
    }

    // Log info
    Packages = malloc(MAX_PACKAGES * sizeof(struct Package*));
    if(analizeLog(argv[2]) == 1) return 1;
    if(generateReport(report_file) == 1) return 1;
    
    return 0;
}
