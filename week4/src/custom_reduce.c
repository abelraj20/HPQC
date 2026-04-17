#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

// check command line arguments
int check_args(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [SIZE]\n", argv[0]);
        exit(-1);
    }

    char *endptr;
    long size = strtol(argv[1], &endptr, 10);
    if (*endptr != '\0' || size <= 0) {
        fprintf(stderr, "SIZE must be a positive integer\n");
        exit(-1);
    }

    return (int)size;
}

// custom sum function for MPI_Op_create
// MPI calls this when combining values during the reduction
void custom_sum(void *invec, void *inoutvec, int *len, MPI_Datatype *datatype) {
    long long *in = (long long *)invec;
    long long *inout = (long long *)inoutvec;

    for (int i = 0; i < *len; i++)
        inout[i] += in[i];
}

int main(int argc, char **argv) {
    int rank, nprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    int size;

    if (rank == 0)
        size = check_args(argc, argv);

    // broadcast size to everyone
    MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // create custom MPI operation
    MPI_Op my_sum_op;
    MPI_Op_create(custom_sum, 1, &my_sum_op);

    // work out each process chunk
    int chunk_size = size / nprocs;
    int remainder = size % nprocs;
    int start = rank * chunk_size + (rank < remainder ? rank : remainder);
    int end = start + chunk_size + (rank < remainder ? 1 : 0);
    int local_size = end - start;

    long long local_sum = 0;
    long long reduce_sum = 0;
    long long custom_sum_result = 0;
    long long expected_sum = ((long long)size * (size + 1)) / 2;

    double t_start, t_end;
    double reduce_time, custom_time;

    // compute local sum for this process
    for (int i = 0; i < local_size; i++)
        local_sum += start + i + 1;

    // predefined MPI reduce
    MPI_Barrier(MPI_COMM_WORLD);
    t_start = MPI_Wtime();

    MPI_Reduce(&local_sum, &reduce_sum, 1, MPI_LONG_LONG_INT,
               MPI_SUM, 0, MPI_COMM_WORLD);

    t_end = MPI_Wtime();
    reduce_time = t_end - t_start;

    // custom MPI reduce
    MPI_Barrier(MPI_COMM_WORLD);
    t_start = MPI_Wtime();

    MPI_Reduce(&local_sum, &custom_sum_result, 1, MPI_LONG_LONG_INT,
               my_sum_op, 0, MPI_COMM_WORLD);

    t_end = MPI_Wtime();
    custom_time = t_end - t_start;

    if (rank == 0) {
        printf("[reduce] Total sum: %lld\n", reduce_sum);
        printf("[reduce] Error: %lld\n", llabs(reduce_sum - expected_sum));
        printf("[reduce] Time: %f seconds\n", reduce_time);

        printf("[customreduce] Total sum: %lld\n", custom_sum_result);
        printf("[customreduce] Error: %lld\n", llabs(custom_sum_result - expected_sum));
        printf("[customreduce] Time: %f seconds\n", custom_time);
    }

    MPI_Op_free(&my_sum_op);
    MPI_Finalize();
    return 0;
}
