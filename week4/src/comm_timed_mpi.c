
#include <stdio.h>
#include <mpi.h>

// function to initialise MPI and get rank & size
void mpi_initialize(int *argc, char ***argv, int *my_rank, int *uni_size)
{
    MPI_Init(argc, argv);
    MPI_Comm_rank(MPI_COMM_WORLD, my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, uni_size);
}

// prototypes for functions called in main
void root_task(int my_rank, int uni_size, int count, int tag);
void client_task(int my_rank, int uni_size, int count, int tag);
void check_task(int my_rank, int uni_size, int count, int tag);
void error_check(int uni_size);

int main(int argc, char **argv)
{
    int ierror = 0;
    int my_rank = 0, uni_size = 0;

    mpi_initialize(&argc, &argv, &my_rank, &uni_size);

    int count = 1;
    int tag = 0;

    check_task(my_rank, uni_size, count, tag);

    ierror = MPI_Finalize();
    return 0;
}

// timed root_task function
void root_task(int my_rank, int uni_size, int count, int tag)
{
    int recv_message, source;
    MPI_Status status;

    for (int their_rank = 1; their_rank < uni_size; their_rank++)
    {
        source = their_rank;

        double start_time = MPI_Wtime();
        MPI_Recv(&recv_message, count, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        double end_time = MPI_Wtime();

        printf("[%d] took %f s to receive from %d\n",
               my_rank, end_time - start_time, source);
    }
}

// timed client_task function
void client_task(int my_rank, int uni_size, int count, int tag)
{
    int dest = 0;
    int send_message = my_rank * 10;

    double start_time, end_time;

    start_time = MPI_Wtime();
    MPI_Ssend(&send_message, count, MPI_INT, dest, tag, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    printf("[%d] took %f s to send (Ssend)\n", my_rank, end_time - start_time);

    start_time = MPI_Wtime();
    MPI_Bsend(&send_message, count, MPI_INT, dest, tag, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    printf("[%d] took %f s to send (Bsend)\n", my_rank, end_time - start_time);

    start_time = MPI_Wtime();
    MPI_Rsend(&send_message, count, MPI_INT, dest, tag, MPI_COMM_WORLD);
    end_time = MPI_Wtime();
    printf("[%d] took %f s to send (Rsend)\n", my_rank, end_time - start_time);

    MPI_Request request;
    start_time = MPI_Wtime();
    MPI_Isend(&send_message, count, MPI_INT, dest, tag, MPI_COMM_WORLD, &request);
    MPI_Wait(&request, MPI_STATUS_IGNORE);
    end_time = MPI_Wtime();
    printf("[%d] took %f s to send (Isend)\n", my_rank, end_time - start_time);
}

// check_task function to differentiate root and client tasks
void check_task(int my_rank, int uni_size, int count, int tag)
{
    if (uni_size > 1)
    {
        if (0 == my_rank)
            root_task(my_rank, uni_size, count, tag);
        else
            client_task(my_rank, uni_size, count, tag);
    }
    else
    {
        error_check(uni_size);
    }
}

// error_check function
void error_check(int uni_size)
{
    printf("Unable to communicate with less than 2 processes. MPI communicator size = %d\n", uni_size);
}
