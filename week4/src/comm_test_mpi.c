#include <stdio.h>
#include <mpi.h>

// function to initialise MPI and get rank & size
void mpi_initialize(int *argc, char ***argv, int *my_rank, int *uni_size)
{
    // intitalise MPI
    MPI_Init(argc, argv);

    // gets the rank and world size
    MPI_Comm_rank(MPI_COMM_WORLD, my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, uni_size);
}

int main(int argc, char **argv) 
{
    // declare and initialise error handling variable
    int ierror = 0;
    
    // declare and initialise rank and size varibles
    int my_rank = 0, uni_size = 0;

    // intitalise MPI using a function
    mpi_initialize(&argc, &argv, &my_rank, &uni_size);

    // creates and initialies transmission variables
    int send_message, recv_message, count, dest, source, tag;
    send_message = recv_message = dest = source = tag = 0;
    count = 1;
    MPI_Status status;
    
    if (uni_size > 1)
    {
        if (0 == my_rank)
        {
            // call root task function
            root_task(my_rank, uni_size, count, tag);
        }
        else
        {
            // sets the destination for the message
            dest = 0; // destination is root

            // creates the message
            send_message = my_rank * 10;

            // sends the message
            MPI_Send(&send_message, count, MPI_INT, dest, tag, MPI_COMM_WORLD);

            // prints the message from the sender
            printf("Hello, I am %d of %d. Sent %d to Rank %d\n",
                    my_rank, uni_size, send_message, dest);
        }
    }
    else
    {
        // prints a warning
        printf("Unable to communicate with less than 2 processes. MPI communicator size = %d\n", uni_size);
    }

    // finalise MPI
    ierror = MPI_Finalize();
    return 0;
}

// function for root rank to receive messages
void root_task(int my_rank, int uni_size, int count, int tag)
{
    int recv_message, source;
    MPI_Status status;

    // iterates through all the other ranks
    for (int their_rank = 1; their_rank < uni_size; their_rank++)
    {
        // sets the source argument to the rank of the sender
        source = their_rank;

        // receives the messages
        MPI_Recv(&recv_message, count, MPI_INT, source, tag, MPI_COMM_WORLD, &status);

        // prints the message from the sender
        printf("Hello, I am %d of %d. Received %d from Rank %d\n",
               my_rank, uni_size, recv_message, source);
    }
}

