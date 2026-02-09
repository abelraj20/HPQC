#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// function declarations
int check_args(int argc, char **argv);
void fill_vector(int vector[], int size);
long long sum_chunk(int vector[], int start, int end);

int main(int argc, char **argv)
{
    int rank, nprocs;
    MPI_Init(&argc, &argv);               // start MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // get process rank
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs); // get number of processes

    int num_arg = 0;
    if (rank == 0)
    {
        num_arg = check_args(argc, argv);
    }

    // broadcast vector size to all processes
    MPI_Bcast(&num_arg, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // determine chunk for this process
    int chunk_size = num_arg / nprocs;
    int remainder = num_arg % nprocs;
    int start = rank * chunk_size + (rank < remainder ? rank : remainder);
    int end   = start + chunk_size + (rank < remainder ? 1 : 0);

    // only rank 0 allocates full vector
    int *vector = NULL;
    if (rank == 0)
    {
        vector = malloc(num_arg * sizeof(int));
        if (!vector)
        {
            fprintf(stderr, "Memory allocation failed\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
        fill_vector(vector, num_arg);
    }

    // each process allocates its chunk
    int local_size = end - start;
    int *local_chunk = malloc(local_size * sizeof(int));
    if (!local_chunk)
    {
        fprintf(stderr, "Memory allocation failed\n");
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    // distribute chunks from rank 0 to all processes
    int *sendcounts = NULL;
    int *displs = NULL;
    if (rank == 0)
    {
        sendcounts = malloc(nprocs * sizeof(int));
        displs = malloc(nprocs * sizeof(int));
        for (int i = 0; i < nprocs; i++)
        {
            int s = num_arg / nprocs + (i < remainder ? 1 : 0);
            sendcounts[i] = s;
            displs[i] = (i * (num_arg / nprocs)) + (i < remainder ? i : remainder);
        }
    }

    MPI_Scatterv(vector, sendcounts, displs, MPI_INT,
                 local_chunk, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // compute local sum
    long long local_sum = 0;
    for (int i = 0; i < local_size; i++)
        local_sum += local_chunk[i];

    // reduce all local sums to total sum on rank 0
    long long total_sum = 0;
    MPI_Reduce(&local_sum, &total_sum, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("Sum: %lld\n", total_sum);
        free(vector);
        free(sendcounts);
        free(displs);
    }

    free(local_chunk);
    MPI_Finalize();
    return 0;
}

// fill vector with 1..n
void fill_vector(int vector[], int size)
{
    for (int i = 0; i < size; i++)
        vector[i] = i + 1;
}

// check that command-line argument is valid
int check_args(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s [POSITIVE_INTEGER]\n", argv[0]);
        exit(-1);
    }
    char *endptr;
    long num = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || num <= 0)
    {
        fprintf(stderr, "Argument must be a positive integer!\n");
        exit(-1);
    }
    return (int)num;
}
