#include <stdio.h>
#include <stdlib.h>

static char daytab[2][13] = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

char *month_name(int n) {

   static char *name[] = {
       "Illegal month",
       "Jan",
       "Feb",
       "Mar",
       "Apr",
       "May",
       "Jun",
       "Jul",
       "Aug",
       "Sep",
       "Oct",
       "Nov",
       "Dec"
   };

   return (n < 1 || n > 12) ? name[0] : name[n];
}

int is_year_leap_year(int year) {
    return year % 4 == 0 && year % 100 != 0 || year % 400 == 0;
}

void month_day(int year, int yearday, int *pmonth, int *pday) {
    int leap = is_year_leap_year(year);
    int counted_days = yearday;

    for (int i = 1; i < 13; i++) {
        if (daytab[leap][i] >= counted_days) {
            *pday = counted_days;
            *pmonth = i;
            return;
        }
        counted_days -= daytab[leap][i];
    }
};

int main(int argc, char **argv) {

    if (argc < 3) {
        printf("Usage: $ ./month_day <year> <yearday>\n");
        return 1;
    }

    int year = atoi(argv[1]);
    int yearday = atoi(argv[2]);

    if (year < 0 || yearday < 1 || yearday > 366 ) {
        printf("Please provide a valid combination of year and day\n");
        return 1;
    }

    int pmonth;
    int pday;
    month_day(year, yearday, &pmonth, &pday);

    char full_date[12];
    int stringFormatcode = sprintf(full_date, "%s %d, %d", month_name(pmonth), pday, year);

    if (stringFormatcode < 0) {
        printf("Error formatting string\n");
        return 1;
    }

    printf("%s\n", full_date);
    return 0;
}
