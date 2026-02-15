## Results and Analysis – Week 4
### Part 1:
#### Step 1:
As the number of MPI processes increases, the program continues to function correctly, but the order that the messages are printed becomes less predictable. When running the program 
with 4, 8, 12 and 16 processors, each client rank successfully sends its value to the root (rank 0) and the root receives one message from every other process. This shows that the 
communication pattern is working as intended and scales well with the number of processors.

However, the order in which the “Sent” messages appear varies between runs and becomes more mixed as more processors are used. This happens because MPI processes run at the same time and 
are scheduled independently by the system. Therefore, there is no definite ordering of when each process executes its print statements. In contrast, rank 0 is seen to display its 
“Received” messages in ascending rank order which shows that it is receiving messages in an ordered loop. Overall, these experiments show the random and asynchronous nature of parallel 
execution in MPI, especially as the processor count increases.

#### Step 2:


#### Step 3:


####Step 4:

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
