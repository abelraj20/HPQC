#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

// declarations
int check_args(int argc, char **argv, char **mode);
void fill_vector(int vector[], int size);

int main(int argc, char **argv)
{
    int rank, nprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int num_arg = 0;
    char *mode = NULL;

    if (rank == 0)
        num_arg = check_args(argc, argv, &mode);

    // broadcast size and mode to all processes
    MPI_Bcast(&num_arg, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int mode_len = 16;
    if (rank != 0)
        mode = malloc(mode_len);
    MPI_Bcast(mode, mode_len, MPI_CHAR, 0, MPI_COMM_WORLD);

    // compute chunks
    int chunk_size = num_arg / nprocs;
    int remainder = num_arg % nprocs;
    int start = rank * chunk_size + (rank < remainder ? rank : remainder);
    int end = start + chunk_size + (rank < remainder ? 1 : 0);
    int local_size = end - start;

    int *vector = NULL;
    int *local_chunk = NULL;

    double t_start = MPI_Wtime();

    // Broadcast version
    if (strcmp(mode, "bcast") == 0)
    {
        if (rank == 0)
        {
            vector = malloc(num_arg * sizeof(int));
            fill_vector(vector, num_arg);
        }
        else
        {
            vector = malloc(num_arg * sizeof(int));
        }

        MPI_Bcast(vector, num_arg, MPI_INT, 0, MPI_COMM_WORLD);

        long long local_sum = 0;
        for (int i = start; i < end; i++)
            local_sum += vector[i];

        long long total_sum = 0;
        MPI_Reduce(&local_sum, &total_sum, 1,
                   MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        double t_end = MPI_Wtime();

        if (rank == 0)
        {
            printf("[BCAST] Sum: %lld\n", total_sum);
            printf("[BCAST] Time: %f seconds\n", t_end - t_start);
        }

        free(vector);
    }

    // Scatter version
    else if (strcmp(mode, "scatter") == 0)
    {
        int *sendcounts = NULL;
        int *displs = NULL;

        if (rank == 0)
        {
            vector = malloc(num_arg * sizeof(int));
            fill_vector(vector, num_arg);

            sendcounts = malloc(nprocs * sizeof(int));
            displs = malloc(nprocs * sizeof(int));

            for (int i = 0; i < nprocs; i++)
            {
                int s = num_arg / nprocs + (i < remainder ? 1 : 0);
                sendcounts[i] = s;
                displs[i] = (i * (num_arg / nprocs)) +
                            (i < remainder ? i : remainder);
            }
        }

        local_chunk = malloc(local_size * sizeof(int));

        MPI_Scatterv(vector, sendcounts, displs, MPI_INT,
                     local_chunk, local_size, MPI_INT,
                     0, MPI_COMM_WORLD);

        long long local_sum = 0;
        for (int i = 0; i < local_size; i++)
            local_sum += local_chunk[i];

        long long total_sum = 0;
        MPI_Reduce(&local_sum, &total_sum, 1,
                   MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        double t_end = MPI_Wtime();

        if (rank == 0)
        {
            printf("[SCATTER] Sum: %lld\n", total_sum);
            printf("[SCATTER] Time: %f seconds\n", t_end - t_start);
            free(vector);
            free(sendcounts);
            free(displs);
        }

        free(local_chunk);
    }

    // DIY version
    else if (strcmp(mode, "diy") == 0)
    {
        int *sendcounts = NULL;
        int *displs = NULL;

        if (rank == 0)
        {
            vector = malloc(num_arg * sizeof(int));
            fill_vector(vector, num_arg);

            sendcounts = malloc(nprocs * sizeof(int));
            displs = malloc(nprocs * sizeof(int));

            for (int i = 0; i < nprocs; i++)
            {
                int s = num_arg / nprocs + (i < remainder ? 1 : 0);
                sendcounts[i] = s;
                displs[i] = (i * (num_arg / nprocs)) +
                            (i < remainder ? i : remainder);
            }
        }

        local_chunk = malloc(local_size * sizeof(int));

        if (rank == 0)
        {
            // copy own chunk
            for (int i = 0; i < local_size; i++)
                local_chunk[i] = vector[i];

            // send to others
            for (int i = 1; i < nprocs; i++)
            {
                MPI_Send(vector + displs[i],
                         sendcounts[i],
                         MPI_INT,
                         i,
                         0,
                         MPI_COMM_WORLD);
            }
        }
        else
        {
            MPI_Recv(local_chunk,
                     local_size,
                     MPI_INT,
                     0,
                     0,
                     MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
        }

        long long local_sum = 0;
        for (int i = 0; i < local_size; i++)
            local_sum += local_chunk[i];

        long long total_sum = 0;
        MPI_Reduce(&local_sum, &total_sum, 1,
                   MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        double t_end = MPI_Wtime();

        if (rank == 0)
        {
            printf("[DIY] Sum: %lld\n", total_sum);
            printf("[DIY] Time: %f seconds\n", t_end - t_start);
            free(vector);
            free(sendcounts);
            free(displs);
        }

        free(local_chunk);
    }

    MPI_Finalize();
    return 0;
}

// helper functions
void fill_vector(int vector[], int size)
{
    for (int i = 0; i < size; i++)
        vector[i] = i + 1;
}

int check_args(int argc, char **argv, char **mode)
{
    if (argc != 3)
    {
        fprintf(stderr,
                "Usage: %s [SIZE] [bcast|scatter|diy]\n",
                argv[0]);
        exit(-1);
    }

    char *endptr;
    long num = strtol(argv[1], &endptr, 10);

    if (*endptr != '\0' || num <= 0)
    {
        fprintf(stderr,
                "SIZE must be a positive integer\n");
        exit(-1);
    }

    if (strcmp(argv[2], "bcast") != 0 &&
        strcmp(argv[2], "scatter") != 0 &&
        strcmp(argv[2], "diy") != 0)
    {
        fprintf(stderr,
                "Mode must be: bcast, scatter, or diy\n");
        exit(-1);
    }

    *mode = malloc(16);
    strncpy(*mode, argv[2], 15);
    (*mode)[15] = '\0';

    return (int)num;
}
