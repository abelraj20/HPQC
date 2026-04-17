## Code Instructions
The source code for this project is located in the `week5/src/` folder, while the compiled executables are stored in the `bin/` folder and outputs are stored in the `output/` folder at the top level of the project.

## Results and Analysis – Week 5
### Part 1:
In this part, both `string_wave.c` and `animate_line_file.py` were modified so that fewer values are hard-coded in the program. In `string_wave.c`, the program was changed to take command line arguments for the number of points on the string, the number of cycles, the number of samples per cycle, and the output file location. This means the user can now control the size and duration of the simulation without editing the code each time. The argument-checking section was also expanded so the program can validate the inputs properly and return clear error messages if the wrong arguments are given.

In `animate_line_file.py`, similar changes were made so the input file and output GIF file are provided through the command line instead of being fixed in the script. Optional arguments were also added for values such as the animation frame rate and the y-axis limit, so the output can be adjusted more easily. The overall aim of these changes was to make both programs more flexible and easier to use from the terminal, while keeping the original structure and comments of the code.

### Part 2:
#### Step 1:


#### Step 2:


#### Step 3:


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
