## Code Instructions
The source code for this project is located in the week2/src/ folder. To compile the MPI programs, navigate to week2/src/ and run `mpicc hello_mpi.c -o ../../bin/hello_mpi` for the hello world program and `mpicc 
vector_parallel.c -o ../../bin/vector_parallel` for the vector summation program; this places the compiled binaries in the bin/ folder, which is two levels above src/. To compile the C programs, navigate to week2/src/ 
and run `gcc hello_mpi_serial.c -o ../../bin/hello_mpi_serial` for the serial version of the hello world program, do the same for `vector_serial.c` and `vector_serial_new.c`. This places the compiled binaries in the 
same location as the MPI binary files. To run the MPI programs, navigate to the root project directory, where bin/ is located. All programs are executed with a numeric argument. The code is run with the command: `time mpirun -np 4 bin/hello_mpi 10` for the hello world program. "-np 4" is the number of processors used for this command (maximum is 16 for the DCU cheetah server) and "10" is the argument, making the code output "Hello, I am x of 10" ten times in a random order where x is a number from 1 to 10. Do the same for `vector_parallel` where the argument n gives the number of elements in a vector and the code outputs the sum from 1 to n. "time" is used to externally benchmark the program for arguments of increasing sizes. To run the C programs, navigate to the root project directory, where bin/ is located. The serial version of the hello world program is executed with a numeric argument, e.g. `time ./bin/hello_mpi_serial 10`. Do the same for `vector_serial.c` (trivial code that converts all elements in the vector to 0) and `vector_serial_new.c` (non-trivial code that sums up all values from 1 to argument n serially). All the source code is in the `src/` folder, and the `bin/` folder should contain the compiled MPI and C binaries. An example of how the directory layout should look 
is shown below.

## Results and Analysis – Week 3
### Part 1:
The runtimes of the serial and MPI versions of the Hello World program showed a clear difference between the serial and parallel versions of the programs. For the serial program, the real time remained almost constant around 0.004–0.005 seconds as seen in Table 1. This happened regardless of the number of simulated ranks due to it always executing on a single process. In contrast, the MPI program’s real time increased slightly as the number of processes grew (4 processes to 16 processes), from 0.417 seconds for 4 processes to a peak of 0.473 seconds for 16 processes. The user and system times for MPI also increased significantly with the process count, exceeding the real time at higher numbers. From Table 1, at 16 processes, the user time is 0.360 seconds and the system time is 0.496 seconds. This gave a total CPU time greater than the clock time. This showed that multiple cores were active at the same time, even for these small workloads. This also showed the overhead introduced by MPI initialisation and process management. Table 1 clearly shows that the serial program is many times faster in real time, thus confirming that parallelisation only provides a performance benefit when the computational workload is big enough to offset the MPI overhead.

Table 1: Real, user and system times for serial execution and MPI parallel execution of the Hello World program for varying numbers of processes
| Program | Processes / Simulated ranks | Real time (s) | User time (s) | System  time (s) |
| :--------: | :-----------------: | :---------------: | :----------------: | :---------------: |
| Serial     | 4               | 0.005             | 0.000              | 0.004             |
| Parallel   | 4               | 0.417             | 0.103              | 0.179             |
| Serial     | 8               | 0.004             | 0.000              | 0.003             |
| Parallel   | 8               | 0.425             | 0.203              | 0.275             |
| Serial     | 12              | 0.004             | 0.000              | 0.003             |
| Parallel   | 12              | 0.449             | 0.311              | 0.340             |
| Serial     | 16              | 0.004             | 0.000              | 0.004             |
| Parallel   | 16              | 0.473             | 0.360              | 0.496             |

### Part 2:
main(): This function starts by initialising MPI and checking the number of processes in the communicator and what ranks they have. It then calls check_args() to see if the argument is entered correctly. Then it calls check_uni_size() to see if there are enough processes to run the program. Lastly, it calls check_task() to give work to each process depending on if it's the root or a client. Then it finalises MPI and ends.

root_task(): This receives messages from all client processes one by one. When it receives them, it adds the value to a running total. When all of them have been collected, it prints the total sum as the final result.

client_task(): The client processes multiply their rank by the argument number to make a message. Afterwards, it sends this value to the root process using MPI communication.

check_args(): This checks that the program was called with only one argument number. If the number isn't given, it shows an error message and then exits. If the argument is correct, it changes it from a string to an integer so it can be used in calculations.

check_uni_size(): This makes sure that the MPI communicator has at least one process. If there is less than that amount, it prints an error and exits. If the universe size is ok, the program keeps going.

check_task(): This decides the role of each process based on its rank. The root process (rank 0), calls root_task() to get messages from all the other processes and then sums them. The client processes (ranks >0), call client_task() to calculate their individual contribution and send it to the root.

Overall logic: The program calculates the sum of each client rank multiplied by the argument number. In mathematical terms, if the total number of processes is N, the result is equal to the argument number multiplied by the sum of the integers from 1 to N-1. The sum is then: arg_num x (1+2+...+(N-1)).

### Part 3:
#### (a) Serial Code Steps (vector_serial.c)

It is a serial C program that creates and processes a vector using helper functions. It reads  a command-line argument that gives the size of the vector. The function "check_args" verifies 
that the user gave only one numerical argument and converts it into an integer. If it's missing, the program prints an error message and exits. This step makes sure the program has a 
valid size. Then, the program allocates memory for the vector using malloc. This creates space in memory for the required number of integers. The function "initialise_vector" then sets 
every element of the vector to zero by looping through each index. After initialisation, the function "sum_vector" processes the vector by looping through all elements and adding them to 
a running total. This total is returned to main and outputted. Lastly, the program frees the allocated memory to avoid memory leaks.

#### (b) Non-trivial Serial Code (vector_serial_new.c)

In this updated program, it can handle larger vectors and sums safely. The "sum_vector" function now uses long long instead of int to prevent integer errors, and the printf in main was 
updated to "%lld" as a result. A "fill_vector" was function also added to fill the vector with consecutive numbers (1, 2, 3,..., n where n is the vector size) instead of just zero. Also, 
atoi was replaced with strtol in "check_args" for safer input validation, making sure only positive integers are accepted. The rest of the program was kept the same as the base code.

#### (c) Parallel Version (vector_parallel.c)

First, the MPI environment is initialised by getting the rank and total number of processes. Only the root process (rank 0) reads the command-line argument and 
allocates the full vector, which is then filled with values similar to the serial version. Next, the vector is split into chunks so each process approximately gets an equal number of 
elements, with any extra sent to higher ranks. Each process allocates memory for its own local chunk. The "MPI_Scatterv" function is used to distribute the chunks from the root to all 
processes. Each process then computes the sum of its chunk locally. These partials are combined using "MPI_Reduce". This collects all local sums and produces the total sum on rank 0. 
Lastly, rank 0 prints the final total sum and all the allocated memory is freed before calling "MPI_Finalize". Four processors were used when running the code for benchmarking.

#### (d) Benchmarking

From table 2, the serial base code (vector_serial) doesn’t actually compute sums, which is why all its sums are zero. The updated serial code (vector_serial_new) correctly computes sums 
up to very large numbers using "long long" and was tested from 10^1 to 10^8. Its real time stays very low (0.004 s) for small vectors and only starts increasing noticeably at 10^7 and 
above. The parallel version (vector_parallel) has higher overhead for small vectors as seen in Table 2. For example, with 10^1 to 10^4 elements, it takes around 0.400 s compared to 
0.004 s for the serial version. This shows that MPI communication and setup increase the runtime for small vectors.

As the vector size grows, the parallel version starts to catch up to the serial version. By 10^7 or 10^8 elements, its real time approaches that of vector_serial_new. Even at 10^8, the 
parallel version is slightly slower in real time (1.137 s vs 0.945 s as seen in Table 1), though it spreads the computation across processes.

Table 2: Real, user and system times of base (vector_serial), non-trivial (vector_serial_new) and parallel (vector_parallel) code for varying numbers of vector elements
| Program | Vector Elements | Sum | Real time (s) | User time (s) | System  time (s) |
| :--------: | :-----------------:| :---------------: | :---------------: | :----------------: | :---------------: |
| vector_serial     | 10^1            | 0                 | 0.004              | 0.004              | 0.000             |
| vector_serial_new | 10^1            | 55                | 0.004              | 0.000              | 0.004             |
| vector_parallel   | 10^1            | 55                | 0.411              | 0.130              | 0.149             |
| vector_serial     | 10^2            | 0                 | 0.004              | 0.000              | 0.003             |
| vector_serial_new | 10^2            | 5050              | 0.004              | 0.203              | 0.004             |
| vector_parallel   | 10^2            | 5050              | 0.414              | 0.119              | 0.156             |
| vector_serial     | 10^3            | 0                 | 0.004              | 0.000              | 0.003             |
| vector_serial_new | 10^3            | 500500            | 0.004              | 0.311              | 0.004             |
| vector_parallel   | 10^3            | 500500            | 0.412              | 0.150              | 0.121             |
| vector_serial     | 10^4            | 0                 | 0.004              | 0.000              | 0.003             |
| vector_serial_new | 10^4            | 50005000          | 0.004              | 0.360              | 0.004             |
| vector_parallel   | 10^4            | 50005000          | 0.405              | 0.141              | 0.132             |
| vector_serial     | 10^5            | 0                 | 0.006              | 0.000              | 0.005             |
| vector_serial_new | 10^5            | 5000050000        | 0.007              | 0.003              | 0.003             |
| vector_parallel   | 10^5            | 5000050000        | 0.412              | 0.134              | 0.149             |
| vector_serial     | 10^6            | 0                 | 0.019              | 0.018              | 0.000             |
| vector_serial_new | 10^6            | 500000500000      | 0.019              | 0.011              | 0.008             |
| vector_parallel   | 10^6            | 500000500000      | 0.421              | 0.155              | 0.153             |
| vector_serial     | 10^7            | 0                 | 0.080              | 0.056              | 0.024             |
| vector_serial_new | 10^7            | 50000005000000    | 0.104              | 0.084              | 0.017             |
| vector_parallel   | 10^7            | 50000005000000    | 0.480              | 0.377              | 0.173             |
| vector_serial     | 10^8            | 0                 | 0.705              | 0.513              | 0.192             |
| vector_serial_new | 10^8            | 5000000050000000  | 0.945              | 0.772              | 0.172             |
| vector_parallel   | 10^8            | 5000000050000000  | 1.137              | 2.437              | 0.541             |

From these results, the parallel version real time does not outperform the non-trivial serial version within the tested range. Even for vector sizes greater than 10^8, it is likely that the parallel code will continue to lag behind on this system. This suggests that the program does not scale well with the current setup and that a larger or more complex parallel system would be required to achieve a clear improvement.

## Directory Layout:
```
project_root/
├── bin/                  # compiled MPI and C code goes here
│   ├── hello_mpi
|   ├── hello_mpi_serial
|   ├── vector_serial
|   ├── vector_serial_new
│   └── vector_parallel
└── week3/
    └── src/              # source code is here
        ├── hello_mpi.c
        ├── hello_mpi_serial.c
        ├── proof.c
        ├── vector_serial.c
        ├── vector_serial_new.c
        └── vector_parallel.c
```
