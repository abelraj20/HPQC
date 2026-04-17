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

### Part 3:

## Directory Layout:
```
project_root/
├── bin/                  # compiled C code goes here
│   ├── time_write
│   └── custom_reduce
├── data/                 # CSV files goes here
│   ├── string_wave.csv
│   └── ...
├── output/               # outputs goes here
│   ├── string.gif
│   └── ...
└── week5/
    └── src/              # source code is here
        ├── string_wave.c
        ├── animate_line.py
        └── animate_line_file.py
```
