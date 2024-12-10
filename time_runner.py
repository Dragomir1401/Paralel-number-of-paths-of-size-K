import os
import time
import subprocess

input_file = "../in/large.txt"
output_dir = "times"
pthreads_output = os.path.join(output_dir, "pthread_times.txt")
mpi_output = os.path.join(output_dir, "mpi_times.txt")
hybrid_output = os.path.join(output_dir, "hybrid_times.txt")
power = 1000000

os.makedirs(output_dir, exist_ok=True)

# Function to execute a command and record the execution time
def run_command(command, output_file):
    start_time = time.time()
    result = subprocess.run(command, shell=True, capture_output=True)
    end_time = time.time()
    elapsed_time = end_time - start_time

    with open(output_file, "a") as f:
        f.write(f"Command: {command}\n")
        f.write(f"Time: {elapsed_time:.2f} seconds\n\n")

    if result.returncode != 0:
        print(f"Error running command: {command}")
        print(result.stderr.decode())
    else:
        print(f"Executed: {command} in {elapsed_time:.2f} seconds")

# Pthreads experiments
print("Running Pthreads experiments...")
for threads in [2, 4, 6]:
    command = f"./Pthreads/matrix {input_file} {power} 1 2 {threads}"
    run_command(command, pthreads_output)

# MPI experiments
print("Running MPI experiments...")
for processes in [2, 4, 6]:
    make_command = "make -f mpi_makefile"
    run_command(make_command, mpi_output)
    command = f"mpirun --oversubscribe -np {processes} ./matrix {input_file} {power} 1 2"
    run_command(command, mpi_output)

# Hybrid experiments
print("Running Hybrid MPI + Pthreads experiments...")
for threads in [2, 4, 8]:
    for processes in [2, 4, 8]:
        make_command = "make -f hybrid_makefile"
        run_command(make_command, hybrid_output)
        command = f"mpirun --oversubscribe -np {processes} ./mpi_pthreads {input_file} {power} 1 2 {threads}"
        run_command(command, hybrid_output)

print("All experiments completed.")
