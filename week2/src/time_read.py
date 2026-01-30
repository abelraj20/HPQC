import sys
import time
import os

def main():
    if len(sys.argv) != 2:
        print("Enter a file from data folder two levels above this directory")
        return

    filepath = os.path.join("..", "..", "data", sys.argv[1])

    start_time = time.time()

    with open(filepath, "r") as f:
        data = f.read()   # read entire file into memory

    end_time = time.time()

    runtime = end_time - start_time
    print(f"\nRuntime for file read: {runtime:.6f} seconds\n")

if __name__ == "__main__":
    main()
