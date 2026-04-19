import numpy as np

workspace = np.array([[1.]])
namestack = []

X_gate = np.array([[0, 1],
                   [1, 0]])

Z_gate = np.array([[1, 0],
                   [0, -1]])

H_gate = np.array([[1, 1],
                   [1, -1]]) * np.sqrt(1/2)


def pushQubit(name, weights):
    global workspace
    global namestack
    if workspace.shape == (1, 1):
        namestack = []
    namestack.append(name)
    weights = weights / np.linalg.norm(weights)
    weights = np.array(weights, dtype=workspace[0, 0].dtype)
    workspace = np.reshape(workspace, (1, -1))
    workspace = np.kron(workspace, weights)


def tosQubit(name):
    global workspace
    global namestack
    k = len(namestack) - namestack.index(name)
    if k > 1:
        namestack.append(namestack.pop(-k))
        workspace = np.reshape(workspace, (-1, 2, 2**(k-1)))
        workspace = np.swapaxes(workspace, -2, -1)


def applyGate(gate, *names):
    global workspace
    if list(names) != namestack[-len(names):]:
        for name in names:
            tosQubit(name)
    workspace = np.reshape(workspace, (-1, 2**(len(names))))
    subworkspace = workspace[:, -gate.shape[0]:]
    np.matmul(subworkspace, gate.T, out=subworkspace)


def probQubit(name):
    global workspace
    tosQubit(name)
    workspace = np.reshape(workspace, (-1, 2))
    prob = np.linalg.norm(workspace, axis=0)**2
    return prob / prob.sum()


def measureQubit(name):
    global workspace
    global namestack
    prob = probQubit(name)
    measurement = np.random.choice(2, p=prob)
    workspace = workspace[:, [measurement]] / np.sqrt(prob[measurement])
    namestack.pop()
    return str(measurement)


def TOFFn_gate(ctl, result):
    applyGate(X_gate, *ctl, result)


def zero_booleanOracle(qubits, result):
    # if all qubits==0 return 1 else return 0
    for qubit in qubits:
        applyGate(X_gate, qubit)
    TOFFn_gate(qubits, result)
    for qubit in qubits:
        applyGate(X_gate, qubit)


def zero_phaseOracle(qubits):
    # if all qubits==0 return -weight else return weight
    for qubit in qubits:
        applyGate(X_gate, qubit)
    applyGate(Z_gate, *namestack)
    for qubit in qubits:
        applyGate(X_gate, qubit)


def sample_phaseOracle(qubits):
    # if all f(x)==1 return -weight else return weight
    applyGate(X_gate, qubits[1])
    applyGate(Z_gate, *namestack)
    applyGate(X_gate, qubits[1])


def groverSearch(n, printProb=True):
    optimalTurns = int(np.pi / 4 * np.sqrt(2**n) - 1/2)
    qubits = list(range(n))

    for qubit in qubits:
        pushQubit(qubit, [1, 1])

    for k in range(optimalTurns):
        sample_phaseOracle(qubits)
        for qubit in qubits:
            applyGate(H_gate, qubit)
        zero_phaseOracle(qubits)
        for qubit in qubits:
            applyGate(H_gate, qubit)
        if printProb:
            print(probQubit(qubits[0]))

    for qubit in reversed(qubits):
        print(measureQubit(qubit), end="")


workspace = np.array([[1.]])
groverSearch(6)
print()
