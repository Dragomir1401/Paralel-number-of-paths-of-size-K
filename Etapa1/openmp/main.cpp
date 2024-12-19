#include <omp.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <cassert>

using namespace std;
using Matrix = vector<vector<long long>>;

const long long MOD = 1e9 + 7;

// Function to multiply two matrices in parallel using OpenMP
void matrixMultiply(const Matrix &A, const Matrix &B, Matrix &result, int n, int numThreads) {
    Matrix tempResult(n, vector<long long>(n, 0));

    #pragma omp parallel for num_threads(numThreads) collapse(2)
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            long long sum = 0;
            for (int k = 0; k < n; ++k) {
                sum = (sum + (A[i][k] * B[k][j]) % MOD) % MOD;
            }
            tempResult[i][j] = sum;
        }
    }

    // Copy the temporary result to the result matrix
    #pragma omp parallel for num_threads(numThreads) collapse(2)
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            result[i][j] = tempResult[i][j];
        }
    }
}

// Function to raise a matrix to a power in parallel using OpenMP
void matrixPower(Matrix &base, int power, int n, int numThreads) {
    Matrix result(n, vector<long long>(n, 0));
    for (int i = 0; i < n; ++i) {
        result[i][i] = 1; // Initialize result as identity matrix
    }

    while (power > 0) {
        if (power % 2 == 1) {
            matrixMultiply(result, base, result, n, numThreads);
        }
        matrixMultiply(base, base, base, n, numThreads);
        power /= 2;
    }

    base = result;
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

    assert(numThreads > 0);

    int n;
    Matrix adjMatrix;

    // Read the input file
    ifstream infile(filename);
    if (!infile) {
        cerr << "Error opening file: " << filename << endl;
        return 1;
    }

    infile >> n;
    adjMatrix = Matrix(n, vector<long long>(n));

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            infile >> adjMatrix[i][j];
        }
    }
    infile.close();

    // Raise the adjacency matrix to the power k
    matrixPower(adjMatrix, k, n, numThreads);

    // Print the result
    cout << "Number of ways of length " << k << " between city " << city_i << " and city " << city_j << " is: ";
    cout << adjMatrix[city_i][city_j] << endl;

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "Execution time: " << duration.count() << " seconds" << endl;

    return 0;
}
