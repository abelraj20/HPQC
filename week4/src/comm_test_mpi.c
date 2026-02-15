#include <stdio.h>
#include <mpi.h>

// prototypes

// function to initialise MPI and get rank & size
void mpi_initialize(int *argc, char ***argv, int *my_rank, int *uni_size)
{
    // intitalise MPI
    MPI_Init(argc, argv);

    // gets the rank and world size
    MPI_Comm_rank(MPI_COMM_WORLD, my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, uni_size);
}

void root_task(int my_rank, int uni_size, int count, int tag);
void client_task(int my_rank, int uni_size, int count, int tag);
void check_task(int my_rank, int uni_size, int count, int tag);  // prototype

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

// root_task definition
void root_task(int my_rank, int uni_size, int count, int tag)
{
    int recv_message, source;
    MPI_Status status;
    for (int their_rank = 1; their_rank < uni_size; their_rank++)
    {
        source = their_rank;
        MPI_Recv(&recv_message, count, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
        printf("Hello, I am %d of %d. Received %d from Rank %d\n",
               my_rank, uni_size, recv_message, source);
    }
}

// client_task definition
void client_task(int my_rank, int uni_size, int count, int tag)
{
    int dest = 0;
    int send_message = my_rank * 10;
    MPI_Send(&send_message, count, MPI_INT, dest, tag, MPI_COMM_WORLD);
    printf("Hello, I am %d of %d. Sent %d to Rank %d\n",
           my_rank, uni_size, send_message, dest);
}

// check_task definition (after client_task)
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
        printf("Unable to communicate with less than 2 processes. MPI communicator size = %d\n", uni_size);
    }
}
