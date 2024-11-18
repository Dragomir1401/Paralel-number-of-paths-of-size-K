#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <cassert>

using namespace std;
using Matrix = vector<long long>;

const int MOD = 1e9 + 7;

// Funcție pentru a transforma un index 2D în 1D într-o matrice alocată contiguă (versiune constantă)
inline const long long &getElement(const Matrix &mat, int i, int j, int n) {
    return mat[i * n + j];
}

// Funcție pentru a transforma un index 2D în 1D într-o matrice alocată contiguă (versiune non-constantă)
inline long long &getElement(Matrix &mat, int i, int j, int n) {
    return mat[i * n + j];
}

// Funcție pentru înmulțirea paralelă a matricilor folosind MPI
void mpiMatrixMultiply(const Matrix &A, const Matrix &B, Matrix &result, int n, int rank, int size) {
    int rowsPerProcess = n / size;
    int startRow = rank * rowsPerProcess;
    int endRow = (rank == size - 1) ? n : startRow + rowsPerProcess;

    Matrix localResult((endRow - startRow) * n, 0);

    // Broadcast pentru întreaga matrice B
    MPI_Bcast(const_cast<long long *>(B.data()), n * n, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    // Calcul local al rândurilor
    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                getElement(localResult, i - startRow, j, n) =
                    (getElement(localResult, i - startRow, j, n) + getElement(A, i, k, n) * getElement(B, k, j, n)) % MOD;
            }
        }
    }

    // Colectare rezultate în procesul root
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

    // Broadcast pentru dimensiunea matricei și exponent
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Alocare contiguă pentru matrice în toate procesele
    if (rank != 0) {
        adjMatrix.resize(n * n);
        resultMatrix.resize(n * n);
    }

    MPI_Bcast(adjMatrix.data(), n * n, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    // Inițializare matrice identitate
    Matrix base = adjMatrix;
    resultMatrix.resize(n * n);
    for (int i = 0; i < n; ++i) {
        getElement(resultMatrix, i, i, n) = 1;
    }

    // Ridicare la putere folosind MPI
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
