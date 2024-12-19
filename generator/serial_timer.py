import subprocess
import time
import matplotlib.pyplot as plt

commands = [
    ["..\\serial\\matrix.exe", "..\\in\\small.txt", "1000000", "100", "1000"],
    ["..\\serial\\matrix.exe", "..\\in\\medium.txt", "1000000", "100", "1000"],
    ["..\\serial\\matrix.exe", "..\\in\\large.txt", "1000000", "100", "1000"]
]
labels = ["small.txt", "medium.txt", "large.txt"]
execution_times = []

for command in commands:
    start_time = time.time()
    subprocess.run(command, shell=True)
    end_time = time.time()
    execution_times.append(end_time - start_time)

plt.figure(figsize=(10, 6))
plt.plot(labels, execution_times, marker='o', linestyle='-', color='b')
plt.title("Execution Time Comparison")
plt.xlabel("Input File")
plt.ylabel("Execution Time (seconds)")
plt.grid(True)
plt.show()
