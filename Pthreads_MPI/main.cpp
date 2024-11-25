#include <mpi.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <cassert>

using namespace std;
using Matrix = vector<vector<long long>>;

const long long MOD = 1e9 + 7;
pthread_barrier_t barrier;

// Structura pentru datele Pthreads
struct ThreadData {
    const Matrix *A;
    const Matrix *B;
    Matrix *tempResult;
    Matrix *result;
    int startRow;
    int endRow;
    int n;
};

// Funcție pentru înmulțirea de matrice folosind Pthreads
void *threadMatrixMultiplyAndCopy(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    const Matrix &A = *data->A;
    const Matrix &B = *data->B;
    Matrix &tempResult = *data->tempResult;
    Matrix &result = *data->result;
    int n = data->n;
    int startRow = data->startRow;
    int endRow = data->endRow;

    // Calcul local
    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < n; ++j) {
            long long sum = 0;
            for (int k = 0; k < n; ++k) {
                sum = (sum + (A[i][k] * B[k][j]) % MOD) % MOD;
            }
            tempResult[i][j] = sum;
        }
    }

    // Barieră pentru sincronizare
    pthread_barrier_wait(&barrier);

    // Copierea rezultatelor
    for (int i = startRow; i < endRow; ++i) {
        for (int j = 0; j < n; ++j) {
            result[i][j] = tempResult[i][j];
        }
    }

    pthread_exit(nullptr);
}

// Funcție de înmulțire paralelă folosind Pthreads
void pthreadMatrixMultiply(const Matrix &A, const Matrix &B, Matrix &result, int n, int numThreads) {
    Matrix tempResult(n, vector<long long>(n, 0));
    vector<pthread_t> threads(numThreads);
    vector<ThreadData> threadData(numThreads);

    pthread_barrier_init(&barrier, nullptr, numThreads);

    int rowsPerThread = n / numThreads;
    int remainingRows = n % numThreads;

    for (int i = 0; i < numThreads; ++i) {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? (startRow + rowsPerThread + remainingRows) : (startRow + rowsPerThread);

        threadData[i] = {&A, &B, &tempResult, &result, startRow, endRow, n};

        pthread_create(&threads[i], nullptr, threadMatrixMultiplyAndCopy, &threadData[i]);
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    pthread_barrier_destroy(&barrier);
}

// Funcție de ridicare la putere paralelă folosind Pthreads
void pthreadMatrixPower(Matrix &base, int power, int n, int numThreads) {
    Matrix result(n, vector<long long>(n, 0));
    for (int i = 0; i < n; ++i) {
        result[i][i] = 1;
    }

    while (power > 0) {
        if (power % 2 == 1) {
            pthreadMatrixMultiply(result, base, result, n, numThreads);
        }
        pthreadMatrixMultiply(base, base, base, n, numThreads);
        power /= 2;
    }

    base = result;
}

// Funcție principală
int main(int argc, char *argv[]) {
    auto start = chrono::high_resolution_clock::now();

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 6) {
        if (rank == 0)
            cerr << "Usage: " << argv[0] << " <input_file> <power> <city_i> <city_j> <num_threads>" << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    string filename = argv[1];
    int totalPower = stoi(argv[2]);
    int city_i = stoi(argv[3]);
    int city_j = stoi(argv[4]);
    int numThreads = stoi(argv[5]);

    assert(numThreads > 0 && (numThreads & (numThreads - 1)) == 0);

    int n;
    Matrix adjMatrix;

    if (rank == 0) {
        ifstream infile(filename);
        if (!infile) {
            cerr << "Error opening file: " << filename << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        infile >> n;
        adjMatrix.resize(n, vector<long long>(n));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                infile >> adjMatrix[i][j];
            }
        }
        infile.close();
    }

    // Broadcast dimensiunea matricei
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0) {
        adjMatrix.resize(n, vector<long long>(n));
    }

    MPI_Bcast(&adjMatrix[0][0], n * n, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    // Fiecare proces calculează o parte din putere
    int localPower = totalPower / size;
    pthreadMatrixPower(adjMatrix, localPower, n, numThreads);

    // Colectare și combinare rezultate
    Matrix finalResult = adjMatrix;
    for (int step = 1; step < size; ++step) {
        Matrix recvMatrix(n, vector<long long>(n, 0));
        if (rank == (step % size)) {
            MPI_Send(&adjMatrix[0][0], n * n, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
        } else if (rank == 0) {
            MPI_Recv(&recvMatrix[0][0], n * n, MPI_LONG_LONG, step % size, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            pthreadMatrixMultiply(finalResult, recvMatrix, finalResult, n, numThreads);
        }
    }

    if (rank == 0) {
        cout << "Number of ways of length " << totalPower << " between city " << city_i << " and city " << city_j << " is: ";
        cout << finalResult[city_i][city_j] << endl;
    }

    MPI_Finalize();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    if (rank == 0) {
        cout << "Execution time: " << duration.count() << " seconds" << endl;
    }

    return 0;
}
