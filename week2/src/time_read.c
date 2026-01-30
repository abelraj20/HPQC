#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double to_second_float(struct timespec in_time);
struct timespec calculate_runtime(struct timespec start_time, struct timespec end_time);

int main(int argc, char **argv)
{
    struct timespec start_time, end_time, time_diff;
    double runtime = 0.0;

    if (argc != 2) {
        fprintf(stderr, "Enter a file from your data folder in this directory\n");
        return -1;
    }

    /* build path to ../../data/<filename> */
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "data/%s", argv[1]);

    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        perror("Error opening file");
        return -1;
    }

    int capacity = 1024;
    int count = 0;
    int *data = malloc(capacity * sizeof(int));
    if (data == NULL) {
        perror("Memory allocation failed");
        fclose(fp);
        return -1;
    }

    timespec_get(&start_time, TIME_UTC);

    int value;
    while (fscanf(fp, "%d,", &value) == 1) {
        if (count == capacity) {
            capacity *= 2;
            data = realloc(data, capacity * sizeof(int));
            if (data == NULL) {
                perror("Reallocation failed");
                fclose(fp);
                return -1;
            }
        }
        data[count++] = value;
    }

    timespec_get(&end_time, TIME_UTC);

    fclose(fp);
    free(data);

    time_diff = calculate_runtime(start_time, end_time);
    runtime = to_second_float(time_diff);

    printf("Runtime for file read: %lf seconds\n\n", runtime);

    return 0;
}

double to_second_float(struct timespec in_time)
{
    return in_time.tv_sec + in_time.tv_nsec / 1e9;
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

