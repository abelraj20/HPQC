#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

// fill vector with 1..n
void fill_vector(int vector[], int size) {
    for (int i = 0; i < size; i++)
        vector[i] = i + 1;
}

// check command line arguments
int check_args(int argc, char **argv, char **mode) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s [SIZE] [sendrecv|gather|reduce]\n", argv[0]);
        exit(-1);
    }

    char *endptr;
    long size = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || size <= 0) {
        fprintf(stderr, "SIZE must be a positive integer\n");
        exit(-1);
    }

    if (strcmp(argv[2], "sendrecv") != 0 &&
        strcmp(argv[2], "gather") != 0 &&
        strcmp(argv[2], "reduce") != 0) {
        fprintf(stderr, "Mode must be: sendrecv, gather, or reduce\n");
        exit(-1);
    }

    *mode = malloc(16);
    strncpy(*mode, argv[2], 15);
    (*mode)[15] = '\0';

    return (int)size;
}

int main(int argc, char **argv) {
    int rank, nprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int size;
    char *mode = NULL;

    if (rank == 0)
        size = check_args(argc, argv, &mode);

    // broadcast size and mode
    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int mode_len = 16;
    if (rank != 0)
        mode = malloc(mode_len);
    MPI_Bcast(mode, mode_len, MPI_CHAR, 0, MPI_COMM_WORLD);

    // compute chunks
    int chunk_size = size / nprocs;
    int remainder = size % nprocs;
    int start = rank * chunk_size + (rank < remainder ? rank : remainder);
    int end = start + chunk_size + (rank < remainder ? 1 : 0);
    int local_size = end - start;

    int *vector = NULL;
    int *local_chunk = malloc(local_size * sizeof(int));

    double t_start = MPI_Wtime();
    long long local_sum = 0, total_sum = 0;

    // Send/Recv version
    if (strcmp(mode, "sendrecv") == 0) {
        if (rank == 0) {
            vector = malloc(size * sizeof(int));
            fill_vector(vector, size);

            // copy own chunk
            for (int i = 0; i < local_size; i++)
                local_chunk[i] = vector[i];

            // receive from clients
            for (int i = 1; i < nprocs; i++) {
                int src_start = i * chunk_size + (i < remainder ? i : remainder);
                int src_size = chunk_size + (i < remainder ? 1 : 0);
                MPI_Recv(vector + src_start, src_size, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }

            // sum on root
            for (int i = 0; i < size; i++)
                total_sum += vector[i];

            free(vector);
        } else {
            // fill local chunk
            for (int i = 0; i < local_size; i++)
                local_chunk[i] = start + i + 1;

            // send to root
            MPI_Send(local_chunk, local_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
    }

    // Gather version
    else if (strcmp(mode, "gather") == 0) {
        if (rank == 0) {
            vector = malloc(size * sizeof(int));
            fill_vector(vector, size);
        }

        // scatter local chunks first
        int *sendcounts = malloc(nprocs * sizeof(int));
        int *displs = malloc(nprocs * sizeof(int));
        for (int i = 0; i < nprocs; i++) {
            sendcounts[i] = chunk_size + (i < remainder ? 1 : 0);
            displs[i] = i * chunk_size + (i < remainder ? i : remainder);
        }

        MPI_Scatterv(vector, sendcounts, displs, MPI_INT,
                     local_chunk, local_size, MPI_INT, 0, MPI_COMM_WORLD);

        // sum locally
        for (int i = 0; i < local_size; i++)
            local_sum += local_chunk[i];

        // gather sums
        long long *all_sums = NULL;
        if (rank == 0)
            all_sums = malloc(nprocs * sizeof(long long));

        MPI_Gather(&local_sum, 1, MPI_LONG_LONG_INT, all_sums, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

        if (rank == 0) {
            for (int i = 0; i < nprocs; i++)
                total_sum += all_sums[i];
            free(all_sums);
            free(vector);
        }

        free(sendcounts);
        free(displs);
    }

    // Reduce version
    else if (strcmp(mode, "reduce") == 0) {
        // fill local chunk
        for (int i = 0; i < local_size; i++)
            local_sum += start + i + 1;

        MPI_Reduce(&local_sum, &total_sum, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    }

    double t_end = MPI_Wtime();
    if (rank == 0) {
        printf("[%s] Total sum: %lld\n", mode, total_sum);
        printf("[%s] Time: %f seconds\n", mode, t_end - t_start);
    }

    free(local_chunk);
    if (rank != 0)
        free(mode);

    MPI_Finalize();
    return 0;
}
