The source code for this project is located in the week2/src/ folder. To compile the C programs, navigate to week2/src/ and run `gcc time_write.c -o ../../bin/time_write` for the write program and `gcc time_read.c -o ../../bin/time_read` for the read program; this places the
compiled binaries in the bin/ folder, which is two levels above src/. To run the C programs, navigate to the root project directory, where bin/ and data/ are located. The write program is executed with a numeric argument, for example `time ./bin/time_write 23`, which generates 
an output file named `c_output_23.txt` in the data/ folder, while the read program is executed with the filename as an argument, for example `time ./bin/time_read c_output_23.txt`. The Python programs are run from the week2/src/ folder: `time_write.py` is executed with a 
numeric argument, for example `time python3 time_write.py 23`, producing `p_output_23.txt` in the data/ folder, and `time_read.py` is executed with the output filename as an argument, for example `time python3 time_read.py p_output_23.txt`. All outputs are stored in the 
`data/` folder, and the `bin/` folder contains the compiled C binaries. An example of how the directory layout should look is shown below.

This code investigates the performance differences between C and Python when performing basic file input/output (I/O) operations. Two pairs of programs were written: one pair to write a sequence of integers to a file and another pair to read the contents of that file back into 
memory. For each language, the runtime of the core loop was measured internally within the program and overall execution time was also measured using the Linux time command. The goal was to observe how runtime scales with increasing input size and to compare the efficiency of 
C and Python for these operations.

In the file-writing experiments, both the C (time_write.c) and Python (time_write.py) programs wrote integers from 0 up to a specified input value into text files. The tests were carried out for input sizes of 10^1, 10^2, 10^3, 10^4 and 10^5, creating files named 
c_output_10.txt, c_output_100.txt, etc. for C and p_output_10.txt, p_output_100.txt, etc. for Python. For the C program, the runtime for small inputs (10^1 – 10^3 numbers) was extremely low, between 0.002 and 0.004 seconds. As the number of numbers written increased to 10^5, 
the runtime increased linearly to approximately 0.021 seconds. The real time measured by the time command closely matched the runtime recorded in the program, indicating that C handles file I/O very efficiently with minimal overhead.

Python showed similar trends, though with slightly higher runtimes. For small input sizes, the core loop completed in roughly the same range as C, between 0.002 and 0.005 seconds. However, as the input grew to 10^5 numbers, the runtime increased to 0.032 seconds, slightly 
slower than the C program. Regardless of the input size of the argument for the Python code, the real time had a value of at least 0.033 seconds. This difference shows the overhead of Python’s interpreter and its higher-level file I/O operations.

A second set of experiments focused on reading data from the files that were previously created. In both languages, the program read the entire contents of the file into memory and measured the runtime for the operation. For the C implementation (time_read.c), file reading was 
extremely fast and scaled approximately linearly with file size. Reading the smallest file (c_output_10.txt) took approximately 0.000035 seconds, while reading the largest file (c_output_100000.txt) took 0.016 seconds. In these cases, the real times were similar to the 
runtimes. This behaviour reflects the efficiency of C’s low-level file I/O and minimal runtime overhead.

The Python file-reading results (time_read.py) showed a different pattern to the file-writing experiments. For very small files(10^1 - 10^2 input sizes), the measured runtime was slower by an order of ~10 compared to C. For an input size of 10^3, its runtime was approximately 
twice as slow as its C counterpart. Finally, for input sizes of 10^4 - 10^5, Python was faster than C by an order of ~10. The code having a real time of 0.032 second was still evident, making it slower overall. This reflected the interpreter and system overhead rather than the 
time to read the file itself.

Overall, C delivers a faster performance for file I/O with lower overheads, especially with large data, while Python remains practical for smaller workloads.

Example Layout:
project_root/
│
├── bin/                  <-- compiled C code goes here
│   ├── time_write
│   └── time_read
│
├── data/                 <-- output files goes here
│   ├── c_output_23.txt
│   ├── p_output_23.txt
│   └── ...
│
└── week2/
    └── src/              <-- source code is here
        ├── time_write.c
        ├── time_read.c
        ├── time_write.py
        └── time_read.py


