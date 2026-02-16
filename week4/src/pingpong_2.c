#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void mpi_initialize(int *argc, char ***argv, int *rank, int *size);
void root_task(int num_pings, int num_bytes);
void client_task(int num_pings, int num_bytes);
void error_check(int size, int argc);

int main(int argc, char **argv)
{
    int rank, size;
    mpi_initialize(&argc, &argv, &rank, &size);
    error_check(size, argc);

    int num_pings = atoi(argv[1]);
    int num_bytes = atoi(argv[2]);

    if (rank == 0)
        root_task(num_pings, num_bytes);
    else
        client_task(num_pings, num_bytes);

    MPI_Finalize();
    return 0;
}

void mpi_initialize(int *argc, char ***argv, int *rank, int *size)
{
    MPI_Init(argc, argv);
    MPI_Comm_rank(MPI_COMM_WORLD, rank);
    MPI_Comm_size(MPI_COMM_WORLD, size);
}

void root_task(int num_pings, int num_bytes)
{
    int num_ints = num_bytes / sizeof(int);
    int *buffer = malloc(num_ints * sizeof(int));
    MPI_Status status;

    for (int i = 0; i < num_ints; i++)
        buffer[i] = 0;

    double start = MPI_Wtime();

    for (int i = 0; i < num_pings; i++) {
        MPI_Send(buffer, num_ints, MPI_INT, 1, 0, MPI_COMM_WORLD);
        MPI_Recv(buffer, num_ints, MPI_INT, 1, 0, MPI_COMM_WORLD, &status);
        buffer[0]++;
    }

    double end = MPI_Wtime();

    double elapsed = end - start;
    double avg = elapsed / num_pings;

    printf("Final counter: %d\n", buffer[0]);
    printf("Message size: %d bytes\n", num_bytes);
    printf("Elapsed time: %f s\n", elapsed);
    printf("Average per ping-pong: %f s\n", avg);

    free(buffer);
}

void client_task(int num_pings, int num_bytes)
{
    int num_ints = num_bytes / sizeof(int);
    int *buffer = malloc(num_ints * sizeof(int));
    MPI_Status status;

    for (int i = 0; i < num_pings; i++) {
        MPI_Recv(buffer, num_ints, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        buffer[0]++;
        MPI_Send(buffer, num_ints, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    free(buffer);
}

void error_check(int size, int argc)
{
    if (size != 2) {
        fprintf(stderr, "Requires exactly 2 MPI processes\n");
        MPI_Finalize();
        exit(1);
    }

    if (argc != 3) {
        fprintf(stderr,
            "Usage: mpirun -np 2 ./pingpong <num_pings> <bytes>\n");
        MPI_Finalize();
        exit(1);
    }
}
