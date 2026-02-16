## Code Instructions


## Results and Analysis – Week 4
### Part 1:
#### Step 1:
As the number of MPI processes increases, the program continues to function correctly, but the order that the messages are printed becomes less predictable. When running the program with 4, 8, 12 and 16 processors, each client rank successfully sends its value to the root (rank 0) and the root receives one message from every other process. This shows that the communication pattern is working as intended and scales well with the number of processors.

However, the order in which the “Sent” messages appear varies between runs and becomes more mixed as more processors are used. This happens because MPI processes run at the same time and are scheduled independently by the system. Therefore, there is no definite ordering of when each process executes its print statements. In contrast, rank 0 is seen to display its “Received” messages in ascending rank order which shows that it is receiving messages in an ordered loop. Overall, these experiments show the random and asynchronous nature of parallel execution in MPI, especially as the processor count increases.

#### Step 2:
In the updated version of `comm_test_mpi.c`, the original main function has been broken down into smaller, easier-to-read functions. The root_task function handles receiving messages for the root process (rank 0), while client_task handles sending messages for the other processes. The check_task function decides whether a process should act as a root or client, and error_check prints a warning if there are fewer than two processes. Prototypes are declared at the top and main just initialises MPI, sets up variables, calls check_task and finalises MPI. This does the same as the original code but makes it cleaner and easier to understand. Commit history of `comm_test_mpi.c` can be seen using the link: "https://github.com/abelraj20/HPQC/commits/main/week4/src/comm_test_mpi.c" for more details.

#### Step 3:
The program was modified to `comm_vars_mpi.c` to use the four variants of the send operation defined in the Message Passing Interface - `MPI_Ssend()`, `MPI_Bsend()`, `MPI_Rsend()` and `MPI_Isend()`. The overall communication pattern remained the same but some of them behaved differently. With `MPI_Ssend()`, communication was very reliable because the call is synchronous and blocks until the matching receive is posted by the root process. This resulted in consistent execution with no runtime issues. `MPI_Bsend()` also worked correctly once buffering was properly configured. `MPI_Rsend()` was more situational since it assumes that a matching receive has already been posted. Its correctness depended on timing and in some runs, there would be an outcome error if the recieve command hadn't been posted. `MPI_Isend()` was the only one that produced an error when trying to output binary files ("error: too few arguments to function ‘MPI_Isend’". This is because it requires more arguments such as `MPI_Request` and `MPI_Wait` as it starts the send asynchronously. Overall, for reliability and reproducibility in this communication test, `MPI_Ssend()` was the most suitable choice because it has clear synchronisation between sender and receiver and reduces the timing-dependant behaviour.

#### Step 4:


<div align="center">
    
Table 1: Real, user and system times for serial execution (hello_mpi_serial.c) and MPI parallel execution (hello_mpi.c) of the Hello World program for varying numbers of processes
| Program (.c) | Processes / Simulated ranks | Real time (s) | User time (s) | System  time (s) |
| :--------: | :-----------------: | :---------------: | :----------------: | :---------------: |
| Serial     | 4               | 0.005             | 0.000              | 0.004             |
| Parallel   | 4               | 0.417             | 0.103              | 0.179             |
| Serial     | 8               | 0.004             | 0.000              | 0.003             |
| Parallel   | 8               | 0.425             | 0.203              | 0.275             |
| Serial     | 12              | 0.004             | 0.000              | 0.003             |
| Parallel   | 12              | 0.449             | 0.311              | 0.340             |
| Serial     | 16              | 0.004             | 0.000              | 0.004             |
| Parallel   | 16              | 0.473             | 0.360              | 0.496             |

</div>

### Part 2:
