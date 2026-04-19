## Code Instructions

The source code for this task is stored in `week7/src/`. Since this is a Python program, there is no compilation step. To run the program, first move into `week7/src/` and use `python3 quantum_part1.py`. The script follows the ENCCS Part 1 tutorial and includes the basic stack-machine instructions `pushQubit`, `applyGate`, `tosQubit` and `measureQubit`, along with the improved named-qubit versions and common gates such as `X`, `H`, `SWAP`, `CNOT` and `TOFF`. Use the command `python3 quantum_part1.py` to run the code in the same directory as the source code.

## Results and Analysis – Week 7
### Part 1:

In this part, the code demonstrates the main ideas from the tutorial. The `pushQubit()` instruction shows how the workspace grows when a new qubit is added, while the `X` gate flips a qubit from `|0⟩` to `|1⟩` and the Hadamard gate creates an equal superposition. The `SWAP` gate and `tosQubit()` both change the qubit order, and the results show that they give the same rearrangement of the workspace.

The measurement output is random, so the exact bitstrings change between runs, but the probabilities are consistent with the theory. For example, after a Hadamard gate the probability of measuring `0` or `1` is `[0.5 0.5]`, and for weights `[0.6,0.8]` the probabilities are `[0.36 0.64]`. The named-qubit version also makes the simulator easier to follow, since qubits can be referred to as `Q1`, `Q2`, and so on. The final Toffoli-equivalent example shows that a more complex gate can be built from simpler gates like `H`, `T`, `Tinv` and `CNOT`. Overall, the program worked as intended and gave a good introduction to how a quantum computer simulator can be implemented with NumPy.

## Directory Layout
```text
project_root/
└── week7/
    └── src/
        └── quantum_part1.py
```
