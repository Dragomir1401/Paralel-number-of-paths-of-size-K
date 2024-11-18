#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <cassert>
#include <bits/stdc++.h>

using namespace std;
using Matrix = vector<vector<long long>>;

const int MOD = 1e9 + 7;

// Parallel matrix multiplication using MPI
void mpiMatrixMultiply(const Matrix &A, const Matrix &B, Matrix &result, int n, int rank, int size)
{
    // Calculate the range of rows for this process
    int rowsPerProcess = n / size;
    int startRow = rank * rowsPerProcess;
    int endRow = (rank == size - 1) ? n : startRow + rowsPerProcess;

    // Local result buffer for this process's portion of rows
    Matrix localResult(endRow - startRow, vector<long long>(n, 0));

    // Broadcast Matrix B to all processes
    for (int i = 0; i < n; ++i)
    {
        MPI_Bcast(const_cast<long long *>(B[i].data()), n, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
    }

    // Perform local computation for assigned rows
    for (int i = startRow; i < endRow; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            for (int k = 0; k < n; ++k)
            {
                localResult[i - startRow][j] = (localResult[i - startRow][j] + A[i][k] * B[k][j]) % MOD;
            }
        }
    }

    // Gather the local results into the final result matrix on the root process
    for (int i = startRow; i < endRow; ++i)
    {
        MPI_Gather(localResult[i - startRow].data(), n, MPI_LONG_LONG,
                   rank == 0 ? result[i].data() : nullptr,
                   n, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
    }
}

int main(int argc, char *argv[])
{
    auto start = chrono::high_resolution_clock::now();

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0 && argc < 5)
    {
        cerr << "Usage: " << argv[0] << " <input_file> <power> <city_i> <city_j>" << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    string filename;
    int k, city_i, city_j, n;
    Matrix adjMatrix, resultMatrix;

    if (rank == 0)
    {
        // Only the root process reads the matrix
        filename = argv[1];
        k = stoi(argv[2]);
        city_i = stoi(argv[3]);
        city_j = stoi(argv[4]);

        ifstream infile(filename);
        if (!infile)
        {
            cerr << "Error opening file: " << filename << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        infile >> n;
        adjMatrix = Matrix(n, vector<long long>(n));
        resultMatrix = Matrix(n, vector<long long>(n, 0));

        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                infile >> adjMatrix[i][j];
            }
        }
        infile.close();
    }

    // Broadcast matrix size and exponent to all processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Broadcast the adjacency matrix to all processes
    if (rank != 0)
    {
        adjMatrix = Matrix(n, vector<long long>(n));
    }
    for (int i = 0; i < n; ++i)
    {
        MPI_Bcast(adjMatrix[i].data(), n, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
    }

    // Perform matrix exponentiation
    Matrix base = adjMatrix;
    resultMatrix = Matrix(n, vector<long long>(n, 0));
    for (int i = 0; i < n; ++i)
    {
        resultMatrix[i][i] = 1; // Initialize result as identity matrix
    }

    while (k > 0)
    {
        if (k % 2 == 1)
        {
            mpiMatrixMultiply(resultMatrix, base, resultMatrix, n, rank, size); // Multiply result * base
        }
        mpiMatrixMultiply(base, base, base, n, rank, size); // Multiply base * base
        k /= 2;
    }

    if (rank == 0)
    {
        cout << "Number of ways of length " << k << " between city " << city_i << " and city " << city_j << " is: ";
        cout << resultMatrix[city_i][city_j] << endl;
    }

    MPI_Finalize();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "Execution time: " << duration.count() << " seconds" << '\n';
    return 0;
}
