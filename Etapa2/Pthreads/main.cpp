#include <pthread.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <bits/stdc++.h>

using namespace std;
using Matrix = vector<vector<long long>>;
ofstream fout("matrix.out");

const long long MOD = 1e9 + 7;
pthread_barrier_t barrier;

// Structure for passing data to threads
struct ThreadData
{
    const Matrix *A;
    const Matrix *B;
    Matrix *tempResult;
    Matrix *result;
    int startRow;
    int endRow;
    int n;
};

void *threadMatrixMultiplyAndCopy(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    const Matrix &A = *data->A;
    const Matrix &B = *data->B;
    Matrix &tempResult = *data->tempResult;
    Matrix &result = *data->result;
    int n = data->n;
    int startRow = data->startRow;
    int endRow = data->endRow;

    // Calcululate the local result
    for (int i = startRow; i < endRow; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            long long sum = 0;
            for (int k = 0; k < n; ++k)
            {
                sum = (sum + (A[i][k] * B[k][j]) % MOD) % MOD;
            }
            tempResult[i][j] = sum;
        }
    }

    // Barrier to synchronize threads
    pthread_barrier_wait(&barrier);

    // Copy the local result to the final result
    for (int i = startRow; i < endRow; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            result[i][j] = tempResult[i][j];
        }
    }

    pthread_exit(nullptr);
}

// Function to multiply two matrices
void pthreadMatrixMultiply(const Matrix &A, const Matrix &B, Matrix &result, int n, int numThreads)
{
    Matrix tempResult(n, vector<long long>(n, 0));
    vector<pthread_t> threads(numThreads);
    vector<ThreadData> threadData(numThreads);

    // Initialise the barrier
    pthread_barrier_init(&barrier, nullptr, numThreads);

    // Spread the rows among threads
    int rowsPerThread = n / numThreads;
    int remainingRows = n % numThreads;

    for (int i = 0; i < numThreads; ++i)
    {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? (startRow + rowsPerThread + remainingRows) : (startRow + rowsPerThread);

        threadData[i] = {&A, &B, &tempResult, &result, startRow, endRow, n};

        // Create the threads
        pthread_create(&threads[i], nullptr, threadMatrixMultiplyAndCopy, &threadData[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < numThreads; ++i)
    {
        pthread_join(threads[i], nullptr);
    }

    // Destroy the barrier
    pthread_barrier_destroy(&barrier);
}



int main(int argc, char *argv[]) {
    auto start = chrono::high_resolution_clock::now();
    if (argc < 6) {
        cerr << "Usage: " << argv[0] << " <input_file> <power> <city_i> <city_j> <num_threads>" << endl;
        return 1;
    }

    string filename = argv[1];
    int k = stoi(argv[2]);
    int city_i = stoi(argv[3]);
    int city_j = stoi(argv[4]);
    int numThreads = stoi(argv[5]);

    int n;
    Matrix adjMatrix, resultMatrix;

    // Read the input file
    ifstream infile(filename);
    if (!infile)
    {
        cerr << "Error opening file: " << filename << endl;
        return 1;
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

    // Initialise the result matrix
    for (int i = 0; i < n; ++i)
    {
        resultMatrix[i][i] = 1;
    }

    Matrix base = adjMatrix;

    int kk = k;

    // Raise the matrix to the power k
    while (k > 0)
    {
        if (k % 2 == 1)
        {
            pthreadMatrixMultiply(resultMatrix, base, resultMatrix, n, numThreads);
        }
        pthreadMatrixMultiply(base, base, base, n, numThreads);
        k /= 2;
    }

    // Print the result
    cout << "Number of ways of length " << kk << " between city " << city_i << " and city " << city_j << " is: ";
    cout << resultMatrix[city_i][city_j] << endl;

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "Execution time: " << duration.count() << " seconds" << '\n';

    return 0;
}
