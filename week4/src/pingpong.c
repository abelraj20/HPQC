#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void mpi_initialize(int *argc, char ***argv, int *my_rank, int *uni_size);
void root_task(int my_rank, int uni_size, int num_pings);
void client_task(int my_rank, int uni_size, int num_pings);
void error_check(int uni_size, int num_pings);

int main(int argc, char **argv)
{
    int my_rank, uni_size;
    mpi_initialize(&argc, &argv, &my_rank, &uni_size);

    if (argc < 2) {
        if (my_rank == 0) {
            printf("Usage: mpirun -np 2 ./pingpong <num_pings>\n");
        }
        MPI_Finalize();
        return 1;
    }

    int num_pings = atoi(argv[1]);
    error_check(uni_size, num_pings);

    if (my_rank == 0)
        root_task(my_rank, uni_size, num_pings);
    else if (my_rank == 1)
        client_task(my_rank, uni_size, num_pings);

    MPI_Finalize();
    return 0;
}

void mpi_initialize(int *argc, char ***argv, int *my_rank, int *uni_size)
{
    MPI_Init(argc, argv);
    MPI_Comm_rank(MPI_COMM_WORLD, my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, uni_size);
}

void root_task(int my_rank, int uni_size, int num_pings)
{
    int counter = 0;
    int client_rank = 1;
    double start_time, end_time, elapsed_time, average_time;
    MPI_Status status;

    start_time = MPI_Wtime();
    while (counter < num_pings) {
        MPI_Send(&counter, 1, MPI_INT, client_rank, 0, MPI_COMM_WORLD); // ping
        MPI_Recv(&counter, 1, MPI_INT, client_rank, 0, MPI_COMM_WORLD, &status); // pong
    }
    end_time = MPI_Wtime();

    elapsed_time = end_time - start_time;
    average_time = elapsed_time / (double)num_pings;

    printf("Final counter: %d\n", counter);
    printf("Total elapsed time: %f s\n", elapsed_time);
    printf("Average time per ping-pong: %f s\n", average_time);
}

void client_task(int my_rank, int uni_size, int num_pings)
{
    int counter;
    int root_rank = 0;
    MPI_Status status;

    for (int i = 0; i < num_pings; i++) {
        MPI_Recv(&counter, 1, MPI_INT, root_rank, 0, MPI_COMM_WORLD, &status); // ping
        counter += 1;
        MPI_Send(&counter, 1, MPI_INT, root_rank, 0, MPI_COMM_WORLD); // pong
    }
}

void error_check(int uni_size, int num_pings)
{
    if (uni_size != 2) {
        if (uni_size > 0) {
            int rank;
            MPI_Comm_rank(MPI_COMM_WORLD, &rank);
            if (rank == 0)
                printf("Error: This program requires exactly 2 MPI processes.\n");
        }
        MPI_Finalize();
        exit(1);
    }
    if (num_pings <= 0) {
        printf("Error: Number of pings must be > 0.\n");
        MPI_Finalize();
        exit(1);
    }
}
