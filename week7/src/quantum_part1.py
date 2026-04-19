import numpy as np

workspace = np.array([[1.]])
namestack = []

def pushQubit(weights):
    global workspace
    workspace = np.reshape(workspace, (1, -1))
    workspace = np.kron(workspace, weights)

def applyGate(gate):
    global workspace
    workspace = np.reshape(workspace, (-1, gate.shape[0]))
    np.matmul(workspace, gate.T, out=workspace)

def tosQubit(k):
    global workspace
    if k > 1:
        workspace = np.reshape(workspace, (-1, 2, 2**(k-1)))
        workspace = np.swapaxes(workspace, -2, -1)

def probQubit():
    global workspace
    workspace = np.reshape(workspace, (-1, 2))
    return np.linalg.norm(workspace, axis=0)**2

def measureQubit():
    global workspace
    prob = probQubit()
    measurement = np.random.choice(2, p=prob)
    workspace = workspace[:, [measurement]] / np.sqrt(prob[measurement])
    return str(measurement)

X_gate = np.array([[0, 1],
                   [1, 0]])

H_gate = np.array([[1, 1],
                   [1, -1]]) * np.sqrt(1/2)

SWAP_gate = np.array([[1, 0, 0, 0],
                      [0, 0, 1, 0],
                      [0, 1, 0, 0],
                      [0, 0, 0, 1]])

TOFF_gate = np.array([[1, 0, 0, 0, 0, 0, 0, 0],
                      [0, 1, 0, 0, 0, 0, 0, 0],
                      [0, 0, 1, 0, 0, 0, 0, 0],
                      [0, 0, 0, 1, 0, 0, 0, 0],
                      [0, 0, 0, 0, 1, 0, 0, 0],
                      [0, 0, 0, 0, 0, 1, 0, 0],
                      [0, 0, 0, 0, 0, 0, 0, 1],
                      [0, 0, 0, 0, 0, 0, 1, 0]])

def pushNamedQubit(name, weights):
    global workspace
    global namestack
    if workspace.shape == (1, 1):
        namestack = []
    namestack.append(name)
    weights = weights / np.linalg.norm(weights)
    weights = np.array(weights, dtype=workspace[0, 0].dtype)
    workspace = np.reshape(workspace, (1, -1))
    workspace = np.kron(workspace, weights)

def tosNamedQubit(name):
    global workspace
    global namestack
    k = len(namestack) - namestack.index(name)
    if k > 1:
        namestack.append(namestack.pop(-k))
        workspace = np.reshape(workspace, (-1, 2, 2**(k-1)))
        workspace = np.swapaxes(workspace, -2, -1)

def applyNamedGate(gate, *names):
    global workspace
    for name in names:
        tosNamedQubit(name)
    workspace = np.reshape(workspace, (-1, gate.shape[0]))
    np.matmul(workspace, gate.T, out=workspace)

def probNamedQubit(name):
    global workspace
    tosNamedQubit(name)
    workspace = np.reshape(workspace, (-1, 2))
    prob = np.linalg.norm(workspace, axis=0)**2
    return prob / prob.sum()

def measureNamedQubit(name):
    global workspace
    global namestack
    prob = probNamedQubit(name)
    measurement = np.random.choice(2, p=prob)
    workspace = workspace[:, [measurement]] / np.sqrt(prob[measurement])
    namestack.pop()
    return str(measurement)

def toffEquiv_gate(q1, q2, q3):
    T_gate = np.array([[1, 0],
                       [0, np.exp(np.pi / -4j)]])
    Tinv_gate = np.array([[1, 0],
                          [0, np.exp(np.pi / 4j)]])
    CNOT_gate = np.array([[1, 0, 0, 0],
                          [0, 1, 0, 0],
                          [0, 0, 0, 1],
                          [0, 0, 1, 0]])
    H_gate_complex = np.array([[1, 1],
                               [1, -1]]) * np.sqrt(1/2)

    applyNamedGate(H_gate_complex, q3)
    applyNamedGate(CNOT_gate, q2, q3)
    applyNamedGate(Tinv_gate, q3)
    applyNamedGate(CNOT_gate, q1, q3)
    applyNamedGate(T_gate, q3)
    applyNamedGate(CNOT_gate, q2, q3)
    applyNamedGate(Tinv_gate, q3)
    applyNamedGate(CNOT_gate, q1, q3)
    applyNamedGate(T_gate, q2)
    applyNamedGate(T_gate, q3)
    applyNamedGate(H_gate_complex, q3)
    applyNamedGate(CNOT_gate, q1, q2)
    applyNamedGate(T_gate, q1)
    applyNamedGate(Tinv_gate, q2)
    applyNamedGate(CNOT_gate, q1, q2)

print("Instruction 1: push")
workspace = np.array([[1.]])
pushQubit([1, 0])
print(workspace)
pushQubit([3/5, 4/5])
print(workspace)
print()

print("Instruction 2: X gate")
workspace = np.array([[1.]])
pushQubit([1, 0])
print("input", workspace)
applyGate(X_gate)
print("output", workspace)
print()

print("Instruction 2: H gate")
workspace = np.array([[1.]])
pushQubit([1, 0])
print("input", workspace)
applyGate(H_gate)
print("output", workspace)
print()

print("Instruction 2: SWAP gate")
workspace = np.array([[1.]])
pushQubit([1, 0])
pushQubit([0.6, 0.8])
print(workspace)
applyGate(SWAP_gate)
print(workspace)
print()

print("Instruction 3: tosQubit")
workspace = np.array([[1.]])
pushQubit([1, 0])
pushQubit([0.6, 0.8])
print(workspace)
tosQubit(2)
print(np.reshape(workspace, (1, -1)))
print()

print("Instruction 4: measureQubit")
workspace = np.array([[1.]])
for n in range(30):
    pushQubit([0.6, 0.8])
    print(measureQubit(), end="")
print()
print()

print("Three-qubit Toffoli sample")
workspace = np.array([[1.]])
for i in range(16):
    pushQubit([1, 0])
    applyGate(H_gate)
    pushQubit([1, 0])
    applyGate(H_gate)
    pushQubit([1, 0])
    applyGate(TOFF_gate)
    q3 = measureQubit()
    q2 = measureQubit()
    q1 = measureQubit()
    print(q1 + q2 + q3, end=",")
print()
print()

print("Improved named version")
workspace = np.array([[1.]])
pushNamedQubit("Q1", [1, 1])
print(np.reshape(workspace, (1, -1)))
print(namestack)
pushNamedQubit("Q2", [0, 1])
print(np.reshape(workspace, (1, -1)))
print(namestack)
print()

print("tosNamedQubit")
print(np.reshape(workspace, (1, -1)))
print(namestack)
tosNamedQubit("Q1")
print(np.reshape(workspace, (1, -1)))
print(namestack)
print()

print("applyNamedGate")
print(np.reshape(workspace, (1, -1)))
print(namestack)
applyNamedGate(H_gate, "Q2")
print(np.reshape(workspace, (1, -1)))
print(namestack)
print()

print("probNamedQubit and measureNamedQubit")
workspace = np.array([[1.]])
namestack = []
pushNamedQubit("Q1", [1, 0])
applyNamedGate(H_gate, "Q1")
print("Q1 probabilities:", probNamedQubit("Q1"))
pushNamedQubit("Q2", [0.6, 0.8])
print("Q2 probabilities:", probNamedQubit("Q2"))
print(measureNamedQubit("Q1"), measureNamedQubit("Q2"))
print()

print("Toffoli equivalent with complex gates")
workspace = np.array([[1.+0j]])
namestack = []
for i in range(16):
    pushNamedQubit("Q1", [1, 1])
    pushNamedQubit("Q2", [1, 1])
    pushNamedQubit("Q3", [1, 0])
    toffEquiv_gate("Q1", "Q2", "Q3")
    print(measureNamedQubit("Q1") + measureNamedQubit("Q2") + measureNamedQubit("Q3"), end=",")
    workspace = np.array([[1.+0j]])
    namestack = []
print()
