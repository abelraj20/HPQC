#include <stdio.h>
#include <stdlib.h>

// function declarations
int check_args(int argc, char **argv);
void initialise_vector(int vector[], int size, int initial);
void fill_vector(int vector[], int size);
void print_vector(int vector[], int size);
long long sum_vector(int vector[], int size);

int main(int argc, char **argv)
{
    int num_arg = check_args(argc, argv);

    int *my_vector = malloc(num_arg * sizeof(int));
    if (my_vector == NULL)
    {
        fprintf(stderr, "Memory error\n");
        exit(-1);
    }

    // initialise to 0 (optional)
    initialise_vector(my_vector, num_arg, 0);

    // fill vector with values 1..n
    fill_vector(my_vector, num_arg);

    // sum the vector (handles large totals)
    long long my_sum = sum_vector(my_vector, num_arg);
    printf("Sum: %lld\n", my_sum);

    free(my_vector);
    return 0;
}

// sums all elements in the vector
long long sum_vector(int vector[], int size)
{
    long long sum = 0;
    for (int i = 0; i < size; i++)
    {
        sum += vector[i];
    }
    return sum;
}

// initialise vector elements to a specific value
void initialise_vector(int vector[], int size, int initial)
{
    for (int i = 0; i < size; i++)
    {
        vector[i] = initial;
    }
}

// fill vector with 1..n
void fill_vector(int vector[], int size)
{
    for (int i = 0; i < size; i++)
    {
        vector[i] = i + 1;
    }
}

// print vector elements (optional, can be skipped for large vectors)
void print_vector(int vector[], int size)
{
    for (int i = 0; i < size; i++)
    {
        printf("%d\n", vector[i]);
    }
}

// check that command-line argument is valid
int check_args(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Enter argument\n");
        fprintf(stderr, "Usage: %s [POSITIVE_INTEGER]\n", argv[0]);
        exit(-1);
    }

    char *endptr;
    long num = strtol(argv[1], &endptr, 10);

    if (*endptr != '\0' || num <= 0)
    {
        fprintf(stderr, "Enter positive integer\n");
        exit(-1);
    }

    return (int)num;
}
