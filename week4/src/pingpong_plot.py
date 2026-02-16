# import relevant libraries
import numpy as np
import matplotlib.pyplot as plt

# message sizes (bytes) from Table 3
sizes = np.array([
    8,
    64,
    512,
    4096,
    32768,
    262144,
    524288,
    1048576,
    1572864, 
    2097152
], dtype=float)

# average time values per ping-pong (s) from Table 3
times = np.array([
    1e-6,
    1e-6,
    1e-6,
    1.1e-5,
    1.7e-5,
    8.0e-5,
    1.49e-4,
    2.83e-4,
    4.23e-4,
    5.59e-4
], dtype=float)

# use formula, y = mx + c
m, c = np.polyfit(sizes, times, 1)

# metrics
latency = c
bandwidth = 1 / m  # bytes per second

print(f"Latency: {latency:.6e} s")
print(f"Bandwidth: {bandwidth/1e6:.2f} MB/s")

# make fitted line
fit_line = m * sizes + c

# cuustomise graph
plt.figure()
plt.plot(sizes, times, 'o', color='black', label='Measured Data', markersize=8) # larger points for readability
plt.plot(sizes, fit_line, color='black', label='Linear Fit')
plt.xlabel('Message Size (Bytes)')
plt.ylabel('Average Ping-Pong Time (s)')
plt.legend()
plt.tight_layout()
# save figure
# plt.savefig("../images/pingpong_plot.png", bbox_inches="tight")

plt.show()
