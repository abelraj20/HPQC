## Code Instructions

The source code for this task is stored in `week9/src/`. This part also requires PyTorch, so install it with `pip3 install torch`. To run the program, first move into `week9/src/` and use `python3 quantum_part3.py`. The script follows the ENCCS Part 3 tutorial and adapts the simulator to PyTorch so the workspace can be stored on either the CPU or GPU. It includes the PyTorch versions of `pushQubit`, `tosQubit`, `applyGate`, `probQubit`, and `measureQubit`, together with the Grover search example. Use the command `python3 quantum_part3.py` to run it.

## Results and Analysis – Week 9
### Part 3:

In this part, the simulator from week 8 was adapted so that the workspace is stored in PyTorch tensors instead of NumPy arrays. This makes it possible to run the same quantum computer simulation on either the CPU or a CUDA-enabled GPU. Most of the changes are replacements of NumPy operations with the equivalent PyTorch operations, like `pt.reshape`, `pt.kron`, `pt.swapaxes`, `pt.matmul`, and `pt.linalg.norm`. The main idea is that the workspace stays in device memory, so there is very little communication needed between the CPU and GPU.

On the Cheetah system, only the CPU version was available, so the program printed `Sorry, only CPU available`. Even so, the Grover search still ran correctly and returned the expected result `1111111111111101`. The recorded CPU runtime was `7.593029 s`, which shows that the PyTorch version still works correctly even without access to a GPU.

Overall, this part shows that quantum computer simulation maps naturally onto GPU-style computation. The code structure remains almost the same as the work in week 8, but moved the workspace to PyTorch.

## Directory Layout

```text
project_root/
└── week9/
    └── src/
        └── quantum_part3.py
```
