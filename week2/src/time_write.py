import sys
import time
import os

def main():
    output = 0
    # checks if there are the right number of arguments
    try:
        # converts the first argument to an integer
        in_arg = int(sys.argv[1])
    except:  # argc != 2 or not an integer
        # raises an error
        raise Exception(
            "Incorrect arguments.\nUsage: python time_write.py [NUM]\n"
            "e.g.\npython time_write.py 10"
        )

    # creates file name
    filename = f"../../data/p_output_{in_arg}.txt"

    # gets the start time for the loop
    start_time = time.time()

    # opens file
    with open(filename, "w") as f:
        # writes the numbers in file
        for i in range(in_arg):
            f.write(f"{i}, ")

    # gets the time after the loop
    end_time = time.time()
    # calculates the runtime
    runtime = end_time - start_time
    # outputs the runtime
    print(f"\n\nRuntime for core loop: {runtime:.6f} seconds.\n")

if __name__ == "__main__":
    main()
