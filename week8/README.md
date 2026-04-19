## Code Instructions

The source code for this task is stored in `week8/src/`. Since this is a Python program, there is no compilation step. To run the program, first move into `week8/src/` and use `python3 quantum_part2.py`. The script follows the ENCCS Part 2 tutorial and implements Grover’s search using the named-qubit simulator from Part 1. It includes the subroutines `zero_booleanOracle`, `zero_phaseOracle`, `sample_phaseOracle`, and `groverSearch`, together with the gates needed for the search such as `X`, `Z`, and `H`. Use the command `python3 quantum_part2.py` to run it.

## Results and Analysis – Week 8
### Part 2:

In this part, the code implements Grover’s search, which is used to find a marked input faster than a naive search. The `zero_booleanOracle()` function checks whether all qubits are zero by negating the inputs, applying the generalized Toffoli operation, and then restoring the inputs. The `zero_phaseOracle()` version does not return a separate result bit, but instead changes the sign of the matching state, which is what Grover’s algorithm needs. The `sample_phaseOracle()` acts as the black-box function being searched, and in this example it is set up so that the target state is `111101`.

The `groverSearch()` loop alternates between the sample phase oracle and the zero phase oracle, with Hadamard gates applied in between. This gradually amplifies the amplitude of the correct solution. As the iterations continue, the probabilities move closer to the target value. For the tutorial example with 6 qubits, the probability of measuring `1` on the first qubit increases from about `0.56` to about `0.98`, showing that the search is converging toward the correct answer. The final measured output is `111101`, which matches the target state described in the tutorial.

This demonstrates the main idea of Grover’s search: the function is effectively evaluated for all inputs at once through the quantum state, but the challenge is increasing the probability of the correct answer enough that it can be measured reliably. The code works as intended and shows the expected amplitude amplification behaviour.

## Directory Layout

```text
project_root/
└── week8/
    └── src/
        └── quantum_part2.py
