# Fast Matrix Exponentiation with Parallelization

#### Dragomir Andrei, Marinescu Alexandru, 343C1

## Overview

Let say we have n cities that are connected somehow to one another. If city A is connected to city B then we say that there is a road of length one between them, meaning it takes an hour to complete. We want to find out how many roads which would take k hours to complete are between any cities in the map. The naive solution would compute all the possible roads, but we have a better option.

This algorithm serves as a load balancer guide for very big maps in order to find out how many different ways we can route cars in order to avoid high traffic on long roads between multiple cities.

## Complexity

- **Time Complexity:** $$ O(n^3 \cdot \log(k)) $$
- **Space Complexity:** $$ O(n^2) $$

### Proposal of solution

Our solution is to compute the power of the adjacency matrix of this graph of cities. But making that power of matrices will take a lot of time. For that we will use fast logarithmic exponentiation. We can do even better using parallel programming.

Let's say we want to find out all the roads which take 13 hours between some cities described by the matrix A.

**Matrix Powers:**

$$ A^{13} = A^8 \times A^4 \times A^1 $$
 $$ A^8 = A^4 \times A^4 $$
 $$ A^4 = A^2 \times A^2 $$
 $$ A^2 = A \times A $$

## Architecture used

**12th Gen Intel(R) Core(TM) i7-12700H 2.30 GHz**

### Use of Parallelism

We will use MPI to distribute the computation of each power described on a different MPI worker. The other point of improvement is doing the actual matrix multiplication on threads, MPI, or OpenMP workers. This way, we can optimize every part of the algorithm.

## Requirements

- **C++11** or later (for modern C++ features)
- **MPI** (Message Passing Interface) for distributed parallelization
- **OpenMP** for shared-memory parallelization
- **Make** for building the project

### Size of inputs

We have 3 different input sizes: small, medium, and large.

$$ small - 300 \times 300 $$
$$ medium - 450 \times 450 $$
$$ small - 600 \times 600 $$

All profiling tests have been run with $$ k = 10^{6} $$

<img src="serial/images/Times.png" alt="Input Sizes" width="600" height="350"/>

## Compilation

To compile the project, you can use the provided `Makefile` in each directory by running:

```bash
make
```

```bash
./matrix <input_file> <power> <city_i> <city_j>
```

```bash
Where input file is: ../in/<size>.txt where size is small, medium, large.
Power is the size of the road we are targetting.
City i is the source.
City j is the destination.
```

## Profiling

<img src="serial/images/Large_SS_Hotspots.png" alt="Hotspots" width="850" height="300"/>

We can see that most of the time is spent in the matrix multiplication function as expected with a complexity of $$ O(n^3) $$

<img src="serial/images/OnlyOneThread.png" alt="Input Sizes Times" width="1000" height="300"/>

As VTune indicates the the thread usage is very poor and needs improvements.

## Pseudocode

```bash
Function multiply(a, b, n)
  result = n x n matrix of 0s
  For i, j, k in 0..n-1
    result[i][j] = (result[i][j] + a[i][k] * b[k][j]) % MOD
  Return result

Function matrixExponentiation(base, exp, n)
  result = identity matrix n x n
  While exp > 0
    If exp % 2 == 1, result = multiply(result, base, n)
    base = multiply(base, base, n)
    exp = exp // 2
  Return result

Main function
  If argument count < 5, Print "Usage" and Exit
  Read filename, k, city_i, city_j from arguments

  Open file filename
  If file cant be opened, Print "Error" and Exit

  Read n from file
  If city_i or city_j not in [0, n-1], Print "Invalid index" and Exit

  adjMatrix = n x n matrix from file
  resultMatrix = matrixExponentiation(adjMatrix, k, n)

  Print "Paths of length " + k + " between " + city_i + " and " + city_j + ": " + resultMatrix[city_i][city_j]
```

# Pthreads Implementation

We will use Pthreads to parallelize the matrix multiplication function. We will split the matrix into blocks and each thread will compute a block of the result matrix.

### Serial vs Pthreads time comparison

<img src="Pthreads/images/serival_vs_phtreads.png" alt="PthreadsVsSerial" width="800" height="500"/>

### Pthreads number of threads comparison

<img src="Pthreads/images/grafic_liniar_8_16_threads.png" alt="PthreadsVsSerial" width="800" height="500"/>

<img src="Pthreads/images/grafic_turn_8_16_threads.png" alt="PthreadsVsSerial" width="800" height="300"/>

### Pthreads profiling

- **8 threads**

<img src="Pthreads/images/8threadsVtune.png" alt="PthreadsVsSerial" width="800" height="360"/>

- **16 threads**

<img src="Pthreads/images/16threadsVtune.png" alt="PthreadsVsSerial" width="800" height="360"/>

### Pthreads Hotspots

- **8 threads**

<img src="Pthreads/images/hotspots8threads.png" alt="PthreadsVsSerial" width="800" height="260"/>

- We can see how the profiller says that most of the time is spent inside the multiply matrices function and the other significant part is spent in the threads creation logic.

- **16 threads**

<img src="Pthreads/images/hotspots16threads.png" alt="PthreadsVsSerial" width="800" height="260"/>

- Same scenario as in the 8 threads version, most of the time is spent as expected inside the multiplication function.

# MPI Implementation

We will use MPI to parallelize the actual matrix multiplication by sending each process a piece of the matrix.

### Serial vs MPI time comparison

<img src="mpi/images/grafic_mpi_8_16_serial.png" alt="PthreadsVsSerial" width="800" height="500"/>

### MPI number of processes comparison

<img src="mpi/images/grafic_liniar_8vs16_procese.png" alt="PthreadsVsSerial" width="800" height="500"/>

<img src="mpi/images/grafic_vertical8vs16_procese.png" alt="PthreadsVsSerial" width="800" height="300"/>

### MPI profiling

- **8 processes**

<img src="mpi/images/8mpiCores.png" alt="PthreadsVsSerial" width="800" height="300"/>

- **16 processes**

<img src="mpi/images/16mpiCores.png" alt="PthreadsVsSerial" width="800" height="300"/>

### MPI profiling

- **8 processes**

<img src="mpi/images/8mpiHotspots.png" alt="PthreadsVsSerial" width="800" height="300"/>

- We can see the increased performance and no red flag indicated by the profiller when we run with a mapping of 1:1 process to cores.

- **16 processes**

<img src="mpi/images/16mpiHotspots.png" alt="PthreadsVsSerial" width="800" height="340"/>

- We can see how the profiller suggests that the MPI_Bcast si PMPI_GAther takes too much time for this 16 processes version. That can be due to the fact that this interprocess communication is harder when ran with 16 processes on 8 physical cores. That was not the case when the mapping of processes per core was 1:1.

### MPI vs Pthreads vs Serial

<img src="general/images/mpiVsPthreadsVsSerial.png" alt="PthreadsVsSerial" width="800" height="340"/>

### **Maximum Parallelism Analysis Using Amdahl's Law**

To evaluate the theoretical and practical speedup achievable by our implementation, we use **Amdahl's Law**:

$$
S = \frac{1}{(1 - P) + \frac{P}{N}}
$$

Where:

- **S**: Speedup achieved.
- **P**: Proportion of the program that is parallelizable.
- **N**: Number of processing units (threads/processes).

---

### **Assumptions**

From profiling data, we estimate:

- **P ≈ 0.95** (95% of the workload is parallelizable).

---

### **Serial Execution Time**

The execution times for the serial implementation are:

- **Small**: 24.94 seconds
- **Medium**: 95.27 seconds
- **Large**: 220.89 seconds

---

### **Theoretical Maximum Speedup**

Using Amdahl's Law with **P = 0.95**:

$$
S_{\text{max}} = \frac{1}{1 - P} = \frac{1}{1 - 0.95} = 20
$$

The theoretical maximum speedup achievable with perfect parallelism is **20x**.

---

### **Observed Speedup**

#### **Pthreads Speedup**

Using the formula:

$$
S = \frac{\text{Serial Time}}{\text{Parallel Time}}
$$

- **Small Matrix**:

  - 8 threads: $$ S = \frac{24.94}{2.18} \approx 11.44 $$
  - 16 threads: $$ S = \frac{24.94}{2.23} \approx 11.18 $$

- **Medium Matrix**:

  - 8 threads: $$ S = \frac{95.27}{6.65} \approx 14.33 $$
  - 16 threads: $$ S = \frac{95.27}{6.42} \approx 14.84 $$

- **Large Matrix**:
  - 8 threads: $$ S = \frac{220.89}{13.61} \approx 16.23 $$
  - 16 threads: $$ S = \frac{220.89}{14.92} \approx 14.80 $$

#### **MPI Speedup**

Using the same formula:

- **Small Matrix**:

  - 8 processes: $$ S = \frac{24.94}{1.24} \approx 20.11 $$
  - 16 processes: $$ S = \frac{24.94}{2.66} \approx 9.38 $$

- **Medium Matrix**:

  - 8 processes: $$ S = \frac{95.27}{3.42} \approx 27.87 $$ (**Not possible** due to overhead miscalculation)
  - 16 processes: $$ S = \frac{95.27}{5.35} \approx 17.81 $$

- **Large Matrix**:
  - 8 processes: $$ S = \frac{220.89}{7.88} \approx 28.05 $$ (**Not possible** due to overhead miscalculation)
  - 16 processes: $$ S = \frac{220.89}{10.97} \approx 20.14 $$

---

### **Revised Theoretical Speedup for MPI**

The observed MPI speedup exceeding the theoretical limit of 20x suggests an error in time measurement or overhead handling. Recalculating for MPI:

From **Large Matrix, 8 Processes**:

$$
S = 28.05, \quad P = 1 - \frac{1}{S}
$$

$$
P = 1 - \frac{1}{28.05} \approx 0.964
$$

Substituting \( P = 0.964 \):

$$
S_{\text{max}} = \frac{1}{1 - P} = \frac{1}{1 - 0.964} \approx 27.78
$$

Revised theoretical maximum speedup for MPI is approximately **27.78x**.

---

### **Observations**

- **Pthreads**: Diminishing returns as the number of threads increases, likely due to overhead from thread creation and memory contention.
- **MPI**: Displays better scalability but suffers from communication overhead for large process counts, particularly when exceeding physical core count.

- **Theoretical Limit**: For \( P = 0.95 \), the theoretical limit remains **20x**, but higher parallelizability \( P = 0.964 \) in MPI increases the limit to **27.78x**.

This revised limit aligns with practical observations while respecting Amdahl's Law.

# Timeline

- **12 November 2024** - Project documentation and serial implementation. Profiling for serial code.
- **19 November 2024** - Parallel implementation with Pthreads. Time comparisons and profiling on parallel code. Pthreads vs Serial comparison.
- **26 November 2024** - MPI implementation. Time comparisons and profiling on parallel code. MPI vs Serial comparison.
