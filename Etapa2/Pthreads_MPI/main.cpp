#include <mpi.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <cassert>

using namespace std;
using Matrix = vector<long long>;

const int MOD = 1e9 + 7;
pthread_barrier_t barrier;

// Structure for the data that is passed to each thread
struct ThreadData
{
    const Matrix *A;
    const Matrix *B;
    Matrix *result;
    int startRow;
    int endRow;
    int n;
};

// Each thread takes a range of rows and multiplies them
void *threadMatrixMultiply(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    const Matrix &A = *data->A;
    const Matrix &B = *data->B;
    Matrix &result = *data->result;
    int n = data->n;
    int startRow = data->startRow;
    int endRow = data->endRow;

    // Calcul local al rândurilor
    for (int i = startRow; i < endRow; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            long long sum = 0;
            for (int k = 0; k < n; ++k)
            {
                sum = (sum + (A[i * n + k] * B[k * n + j]) % MOD) % MOD;
            }
            result[i * n + j] = sum;
        }
    }

    pthread_exit(nullptr);
}

// Parallel matrix multiplication using Pthreads
void pthreadMatrixMultiply(const Matrix &A, const Matrix &B, Matrix &result, int n, int numThreads)
{
    vector<pthread_t> threads(numThreads);
    vector<ThreadData> threadData(numThreads);

    int rowsPerThread = n / numThreads;
    int remainingRows = n % numThreads;

    for (int i = 0; i < numThreads; ++i)
    {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? (startRow + rowsPerThread + remainingRows) : (startRow + rowsPerThread);

        threadData[i] = {&A, &B, &result, startRow, endRow, n};

        pthread_create(&threads[i], nullptr, threadMatrixMultiply, &threadData[i]);
    }

    for (int i = 0; i < numThreads; ++i)
    {
        pthread_join(threads[i], nullptr);
    }
}

// Pthreads function for matrix power
void pthreadMatrixPower(Matrix &base, int power, int n, int numThreads)
{
    Matrix result(n * n, 0);
    for (int i = 0; i < n; ++i)
    {
        result[i * n + i] = 1;
    }

    while (power > 0)
    {
        if (power % 2 == 1)
        {
            Matrix temp = result;
            pthreadMatrixMultiply(temp, base, result, n, numThreads);
        }
        Matrix temp = base;
        pthreadMatrixMultiply(temp, temp, base, n, numThreads);
        power /= 2;
    }

    base = result;
}

int main(int argc, char *argv[])
{
    auto start = chrono::high_resolution_clock::now();

    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc < 6)
    {
        if (rank == 0)
            cerr << "Usage: " << argv[0] << " <input_file> <power> <city_i> <city_j> <num_threads>" << endl;
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    string filename = argv[1];
    int totalPower = stoi(argv[2]);
    int city_i = stoi(argv[3]);
    int city_j = stoi(argv[4]);
    int numThreads = stoi(argv[5]);

    assert(numThreads > 0);

    int n;
    Matrix adjMatrix;

    if (rank == 0)
    {
        ifstream infile(filename);
        if (!infile)
        {
            cerr << "Error opening file: " << filename << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        infile >> n;
        adjMatrix.resize(n * n);
        for (int i = 0; i < n; ++i)
        {
            for (int j = 0; j < n; ++j)
            {
                infile >> adjMatrix[i * n + j];
            }
        }
        infile.close();
    }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0)
    {
        adjMatrix.resize(n * n);
    }

    MPI_Bcast(adjMatrix.data(), n * n, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    int localPower = totalPower / size;
    pthreadMatrixPower(adjMatrix, localPower, n, numThreads);

    if (rank == 0)
    {
        Matrix finalResult = adjMatrix;

        for (int i = 1; i < size; ++i)
        {
            Matrix recvMatrix(n * n, 0);
            MPI_Recv(recvMatrix.data(), n * n, MPI_LONG_LONG, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            Matrix temp = finalResult;
            pthreadMatrixMultiply(temp, recvMatrix, finalResult, n, numThreads);
        }

        cout << "Number of ways of length " << totalPower << " between city " << city_i << " and city " << city_j << " is: ";
        cout << finalResult[city_i * n + city_j] << endl;
    }
    else
    {
        MPI_Send(adjMatrix.data(), n * n, MPI_LONG_LONG, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    if (rank == 0)
    {
        cout << "Execution time: " << duration.count() << " seconds" << endl;
    }

    return 0;
}
