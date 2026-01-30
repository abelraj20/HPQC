#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double to_second_float(struct timespec in_time);
struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time);

int main(int argc, char **argv)
{
    // creates and initialises the variables
    int i, input;
    i = input = 0;
    struct timespec start_time, end_time, time_diff;
    double runtime = 0.0;

    // checks if there are the right number of arguments
    if (argc == 2)
    {
        // converts the first argument to an integer
        input = atoi(argv[1]);
    }
    else
    {
        // raises an error
        fprintf(stderr, "Incorrect arguments. Usage: time_write [NUM]\n");
        exit(-1);
    }

    // gets the time before the loop
    timespec_get(&start_time, TIME_UTC);

    // creates file name
    char filename[256];  // buffer for the filename
    snprintf(filename, sizeof(filename), "data/c_output_%d.txt", input);

    // opens file 
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("Error opening file");
        return -1;
    }

    // writes numbers to the file
    for (i = 0; i < input; i++)
    {
        fprintf(fp, "%d, ", i);
    }

    fclose(fp);

    // gets the time after the loop
    timespec_get(&end_time, TIME_UTC);

    // calculates the runtime
    time_diff = calculate_runtime(start_time, end_time);
    runtime = to_second_float(time_diff);

    // outputs the runtime
    printf("\n\nRuntime for core loop: %lf seconds.\n\n", runtime);

    return 0;
}

double to_second_float(struct timespec in_time)
{
    double out_time = in_time.tv_sec + in_time.tv_nsec / 1e9;
    return out_time;
}

struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time)
{
    struct timespec time_diff;
    time_diff.tv_sec = end_time.tv_sec - start_time.tv_sec;
    time_diff.tv_nsec = end_time.tv_nsec - start_time.tv_nsec;

    if (time_diff.tv_nsec < 0) {
        time_diff.tv_sec -= 1;
        time_diff.tv_nsec += 1000000000L;
    }

    return time_diff;
}
