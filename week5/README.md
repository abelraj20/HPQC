## Code Instructions
The source code for this project is located in the `week5/src/` folder, while the compiled executables are stored in the `bin/` folder and outputs are stored in the `output/` folder at the top level of the project.

## Results and Analysis – Week 5
### Part 1:
In this part, both `string_wave.c` and `animate_line_file.py` were modified so that fewer values are hard-coded in the program. In `string_wave.c`, the program was changed to take command line arguments for the number of points on the string, the number of cycles, the number of samples per cycle and the output file location. This means the user can now control the size and duration of the simulation without editing the code each time. The argument-checking section was also expanded so the program can validate the inputs properly and return clear error messages if the wrong arguments are given.

In `animate_line_file.py`, similar changes were made so the input file and output GIF file are provided through the command line instead of being fixed in the script. Optional arguments were also added for values such as the animation frame rate and the y-axis limit, so the output can be adjusted more easily. The overall aim of these changes was to make both programs more flexible and easier to use from the terminal, while keeping the original structure and comments of the code.

### Part 2:
#### Step 1:
For the parallel strategy, the main pattern in `string_wave.c` is that the simulation moves forward one time step at a time and each new position depends on the previous state of the string. As a result, the loop over time steps still has to be carried out sequentially, since the result at time `t+1` depends on the values at time `t`. However, within a single time step, the string can be split into chunks along its length, with each MPI process handling one chunk. 

Most of the update inside each chunk can then be done independently, except at the boundary between neighbouring chunks. Since each point copies the value of the previous point, the first element in a chunk needs the last value from the chunk to its left, so neighbouring processes have to exchange one boundary value each time step. Rank 0 is different because its first element is set by the `driver()` function rather than by a neighbour.

#### Step 2:
For the aggregation strategy, it is not safe to let every MPI process write to the same output file at the same time, because that would make the file unpredictable. A solution is to gather the local chunks back to rank 0 in memory at each time step using `MPI_Gatherv()` and then only rank 0 writes the full line to the CSV file. This is easier to control than having every process write to separate files and merging them later. Small repeated file writes from multiple processes usually add a lot of unnecessary overhead, so gathering in memory and writing once from a single process is a better option for this program.

#### Step 3:
A new MPI version of the program was made by modifying it so it can run across multiple processes. The program now uses `MPI_Init()` and `MPI_Finalize()` to start and end MPI and each process works on a local chunk of the string instead of the whole array. At every time step, neighbouring ranks exchange one value so the chunk boundaries are updated correctly and then the local results are gathered back to rank 0 for output.

#### Part 4:
For small simulations, the serial version performs much better than the MPI version. At 100 points, the serial code runs in 0.016 s compared to 0.441 s for the MPI version, and the same pattern holds at 1000, 5000 and 10000 points as seen in Table 1. This shows that, at these sizes, the communication, gathering and file-writing overhead in the MPI version is larger than the benefit of dividing the work between processes.

As the number of points increases, the gap between the two versions becomes smaller. At 50000 points, the serial version takes 1.937 s while the parallel version takes 2.501 s, and by 500000 points the MPI version becomes slightly faster, taking 16.992 s compared to 17.568 s for the serial code. This shows that parallelisation only becomes applicable once the problem size is big enough for the extra computation to outweigh the MPI overhead.

The user and system times are also noticeably higher for the MPI runs, which shows the cost of message passing between neighbouring ranks and gathering it back to rank 0 at every time step. Overall, the results show that the MPI implementation works, but it is only useful for larger simulations.

<div align="center">
    
Table 1: Real, user and system times for serial execution (`string_wave.c`) and MPI parallel execution (`string_wave_mpi.c`) with 4 processes, for varying numbers of sizes

| Program (.c) | Simulated points | Real time (s) | User time (s) | System time (s) |
| :--------: | :--------------: | :-----------: | :-----------: | :-------------: |
| Serial     | 100              | 0.016         | 0.008         | 0.004           |
| Parallel   | 100              | 0.441         | 0.181         | 0.146           |
| Serial     | 1000             | 0.052         | 0.037         | 0.000           |
| Parallel   | 1000             | 0.466         | 0.256         | 0.161           |
| Serial     | 5000             | 0.227         | 0.135         | 0.008           |
| Parallel   | 5000             | 0.617         | 0.669         | 0.196           |
| Serial     | 10000            | 0.504         | 0.344         | 0.036           |
| Parallel   | 10000            | 0.848         | 1.312         | 0.196           |
| Serial     | 50000            | 1.937         | 1.316         | 0.052           |
| Parallel   | 50000            | 2.501         | 5.933         | 0.247           |
| Serial     | 100000           | 3.772         | 2.585         | 0.126           |
| Parallel   | 100000           | 4.465         | 11.743        | 0.365           |
| Serial     | 200000           | 7.593         | 5.205         | 0.246           |
| Parallel   | 200000           | 8.407         | 23.125        | 0.492           |
| Serial     | 500000           | 17.568        | 11.614        | 0.534           |
| Parallel   | 500000           | 16.992        | 59.026        | 0.816           |

</div>

At 50000 points, increasing the number of MPI processes does not improve the runtime. For 1 process, the run takes 2.409 s, while the 2, 4 and 8 process runs remain very similar, and the 16 process run is slower again at 2.799 s as seen in Table 2. This shows that adding more processes does not make the program faster.

Although the computation is split into smaller chunks, the communication overhead also increases. At each time step, neighbouring processes must exchange boundary values and rank 0 must gather the full string before writing to the output file. For a size of 50000 points, the overhead is too large to benefit from using more processes.

<div align="center">
    
Table 2: Real, user and system times for MPI parallel execution (`string_wave_mpi.c`) at 50,000 simulated points, for varying numbers of processes

| Program (.c) | Processes | Real time (s) | User time (s) | System time (s) |
| :--------: | :-------: | :-----------: | :-----------: | :-------------: |
| Parallel   | 1         | 2.409         | 1.458         | 0.136           |
| Parallel   | 2         | 2.484         | 2.925         | 0.232           |
| Parallel   | 4         | 2.470         | 5.953         | 0.300           |
| Parallel   | 8         | 2.537         | 12.288        | 0.553           |
| Parallel   | 16        | 2.799         | 28.906        | 0.766           |

</div>

### Part 3


## Directory Layout:
```
project_root/
├── bin/                  # compiled C code goes here
│   ├── time_write
│   └── custom_reduce
├── data/                 # CSV files go here
│   ├── string_wave.csv
│   └── ...
└── week5/
    ├── output/           # outputs go here
    │   ├── string.gif
    │   └── ...
    └── src/              # source code is here
        ├── string_wave.c
        ├── animate_line.py
        └── animate_line_file.py
```
