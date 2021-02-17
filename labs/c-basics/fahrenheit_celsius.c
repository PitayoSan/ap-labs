#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc == 2) {
        int fahr = atoi(argv[1]);
        printf("Fahrenheit: %d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
        return 0;
    } else if (argc == 4){
        float start = atof(argv[1]);
        float end = atof(argv[2]);
        float increment = atof(argv[3]);
        int fahr;

        for (fahr = start; fahr <= end; fahr = fahr + increment)
            printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr, (5.0/9.0)*(fahr-32));
        return 0;
    }
    return 1;
}