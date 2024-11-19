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

- **16 threads**

<img src="Pthreads/images/hotspots16threads.png" alt="PthreadsVsSerial" width="800" height="260"/>


# MPI Implementation

We will use MPI to parallelize the actual matrix multiplication by sending each process a piece of the matrix.

### Serial vs MPI time comparison

<img src="mpi/images/grafic_mpi_8_16_serial.png" alt="PthreadsVsSerial" width="800" height="500"/>

### MPI number of processes comparison

<img src="mpi/images/grafic_liniar_8vs16_procese.png" alt="PthreadsVsSerial" width="800" height="500"/>

<img src="mpi/images/grafic_vertical8vs16_procese.png" alt="PthreadsVsSerial" width="800" height="300"/>

### MPI profiling

- **8 threads**

<img src="mpi/images/8mpiCores.png" alt="PthreadsVsSerial" width="800" height="300"/>

- **16 threads**

<img src="mpi/images/16mpiCores.png" alt="PthreadsVsSerial" width="800" height="300"/>

### MPI profiling

- **8 processes**

<img src="mpi/images/8mpiHotspots.png" alt="PthreadsVsSerial" width="800" height="300"/>

- **16 processes**

<img src="mpi/images/16mpiHotspots.png" alt="PthreadsVsSerial" width="800" height="340"/>

# Timeline

- **12 November 2024** - Project documentation and serial implementation. Profiling for serial code.
- **19 November 2024** - Parallel implementation with Pthreads. Time comparisons and profiling on parallel code. Pthreads vs Serial comparison.
- **26 November 2024** - MPI implementation. Time comparisons and profiling on parallel code. MPI vs Serial comparison.
