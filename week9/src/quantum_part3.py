import time
import numpy as np
import torch as pt

pt.autograd.set_grad_enabled(False)

if pt.cuda.is_available():
    print("GPU available")
else:
    print("Sorry, only CPU available")

workspace = None
namestack = []

def pushQubit(name, weights):
    global workspace
    global namestack
    if (workspace.shape[0], workspace.shape[1]) == (1, 1):
        namestack = []
    namestack.append(name)
    weights = weights / np.linalg.norm(weights)
    weights = pt.tensor(weights, device=workspace.device, dtype=workspace[0, 0].dtype)
    workspace = pt.reshape(workspace, (1, -1))
    workspace = pt.kron(workspace, weights)

def tosQubit(name):
    global workspace
    global namestack
    k = len(namestack) - namestack.index(name)
    if k > 1:
        namestack.append(namestack.pop(-k))
        workspace = pt.reshape(workspace, (-1, 2, 2 ** (k - 1)))
        workspace = pt.swapaxes(workspace, -2, -1)

def applyGate(gate, *names):
    global workspace
    if list(names) != namestack[-len(names):]:
        for name in names:
            tosQubit(name)
    workspace = pt.reshape(workspace, (-1, 2 ** len(names)))
    subworkspace = workspace[:, -gate.shape[0]:]
    gate = pt.tensor(gate.T, device=workspace.device, dtype=workspace[0, 0].dtype)
    if workspace.device.type == "cuda":
        pt.matmul(subworkspace, gate, out=subworkspace)
    else:
        subworkspace[:, :] = pt.matmul(subworkspace, gate)

def probQubit(name):
    global workspace
    tosQubit(name)
    workspace = pt.reshape(workspace, (-1, 2))
    prob = pt.linalg.norm(workspace, axis=0) ** 2
    prob = pt.Tensor.cpu(prob).numpy()
    return prob / prob.sum()

def measureQubit(name):
    global workspace
    global namestack
    prob = probQubit(name)
    measurement = np.random.choice(2, p=prob)
    workspace = workspace[:, [measurement]] / np.sqrt(prob[measurement])
    namestack.pop()
    return str(measurement)

X_gate = np.array([[0, 1],
                   [1, 0]])

H_gate = np.array([[1, 1],
                   [1, -1]]) * np.sqrt(1 / 2)

Z_gate = H_gate @ X_gate @ H_gate

def sample_phaseOracle(qubits):
    applyGate(X_gate, qubits[1])
    applyGate(Z_gate, *namestack)
    applyGate(X_gate, qubits[1])

def zero_phaseOracle(qubits):
    [applyGate(X_gate, q) for q in qubits]
    applyGate(Z_gate, *namestack)
    [applyGate(X_gate, q) for q in qubits]

def groverSearch(n, printProb=True):
    qubits = list(range(n))
    [pushQubit(q, [1, 1]) for q in qubits]
    for k in range(int(np.pi / 4 * np.sqrt(2 ** n) - 1 / 2)):
        sample_phaseOracle(qubits)
        [applyGate(H_gate, q) for q in qubits]
        zero_phaseOracle(qubits)
        [applyGate(H_gate, q) for q in qubits]
        if printProb:
            print(probQubit(qubits[0]))
    [print(measureQubit(q), end="") for q in reversed(qubits)]

def run_grover_on_device(n, device, label):
    global workspace
    global namestack

    workspace = pt.tensor([[1.]], device=device, dtype=pt.float32)
    namestack = []

    if device.type == "cuda":
        pt.cuda.synchronize()

    t0 = time.perf_counter()
    groverSearch(n, printProb=False)

    if device.type == "cuda":
        pt.cuda.synchronize()

    t1 = time.perf_counter()
    print(f"\nWith {label}: {t1 - t0:.6f} s")

if __name__ == "__main__":
    if pt.cuda.is_available():
        run_grover_on_device(16, pt.device("cuda"), "GPU")
    run_grover_on_device(16, pt.device("cpu"), "CPU")
