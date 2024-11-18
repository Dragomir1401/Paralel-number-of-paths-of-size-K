import subprocess
import time
import matplotlib.pyplot as plt

# Define the commands and labels
commands = [
    ["..\\pthreads\\matrix.exe", "..\\in\\small.txt", "1000000", "100", "1000"],
    ["..\\pthreads\\matrix.exe", "..\\in\\medium.txt", "1000000", "100", "1000"],
    ["..\\pthreads\\matrix.exe", "..\\in\\large.txt", "1000000", "100", "1000"]
]
labels = ["small.txt", "medium.txt", "large.txt"]
execution_times = []

# Run each command and measure the execution time
for command in commands:
    start_time = time.time()
    subprocess.run(command, shell=True)
    end_time = time.time()
    execution_times.append(end_time - start_time)

# Plotting the results
plt.figure(figsize=(10, 6))
plt.plot(labels, execution_times, marker='o', linestyle='-', color='b')
plt.title("Execution Time Comparison")
plt.xlabel("Input File")
plt.ylabel("Execution Time (seconds)")
plt.grid(True)
plt.show()
