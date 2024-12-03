#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <cassert>

using namespace std;
using Matrix = vector<long long>;

const int MOD = 1e9 + 7;

// Function to put the matrix as a 1D array 
inline const long long &getElement(const Matrix &mat, int i, int j, int n) {
    return mat[i * n + j];
}

// Function to put the matrix as a 1D array
inline long long &getElement(Matrix &mat, int i, int j, int n) {
    return mat[i * n + j];
}

// Function to multiply two matrices
void mpiMatrixMultiply(const Matrix &A, const Matrix &B, Matrix &result, int n, int rank, int size) {
    // Spread the work among processes
    int rowsPerProcess = (n + size - 1) / size;
    int startRow = rank * rowsPerProcess;
    int endRow = min(startRow + rowsPerProcess, n);

    Matrix localResult((endRow - startRow) * n, 0);

    // Broadcast the whole B matrix
    MPI_Bcast(const_cast<long long *>(B.data()), n * n, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    // Calculate the local result
    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                getElement(localResult, i - startRow, j, n) =
                    (getElement(localResult, i - startRow, j, n) + getElement(A, i, k, n) * getElement(B, k, j, n)) % MOD;
            }
        }
    }

    // Collect the results
    MPI_Gather(localResult.data(), (endRow - startRow) * n, MPI_LONG_LONG,
               rank == 0 ? result.data() : nullptr, (endRow - startRow) * n, MPI_LONG_LONG,
               0, MPI_COMM_WORLD);
}

int main(int argc, char *argv[]) {
    auto start = chrono::high_resolution_clock::now();

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0 && argc < 5) {
        cerr << "Usage: " << argv[0] << " <input_file> <power> <city_i> <city_j>" << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    string filename;
    int k, city_i, city_j, n;
    Matrix adjMatrix, resultMatrix;

    if (rank == 0) {
        filename = argv[1];
        k = stoi(argv[2]);
        city_i = stoi(argv[3]);
        city_j = stoi(argv[4]);

        ifstream infile(filename);
        if (!infile) {
            cerr << "Error opening file: " << filename << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        infile >> n;
        adjMatrix.resize(n * n);
        resultMatrix.resize(n * n);

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                infile >> getElement(adjMatrix, i, j, n);
            }
        }
        infile.close();
    }

    // Broadcast the input data
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Allocate memory for the matrices
    if (rank != 0) {
        adjMatrix.resize(n * n);
        resultMatrix.resize(n * n);
    }

    MPI_Bcast(adjMatrix.data(), n * n, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    //Initialize the result matrix
    Matrix base = adjMatrix;
    resultMatrix.resize(n * n);
    for (int i = 0; i < n; ++i) {
        getElement(resultMatrix, i, i, n) = 1;
    }

    // Raise the adjacency matrix to the power of k
    int originalK = k;
    while (k > 0) {
        if (k % 2 == 1) {
            mpiMatrixMultiply(resultMatrix, base, resultMatrix, n, rank, size);
        }
        mpiMatrixMultiply(base, base, base, n, rank, size);
        k /= 2;
    }

    if (rank == 0) {
        cout << "Number of ways of length " << originalK << " between city " << city_i << " and city " << city_j << " is: ";
        cout << getElement(resultMatrix, city_i, city_j, n) << endl;
    }

    MPI_Finalize();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    if (rank == 0) {
        cout << "Execution time: " << duration.count() << " seconds" << '\n';
    }

    return 0;
}
