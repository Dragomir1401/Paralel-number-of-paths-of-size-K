#include <pthread.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>

using namespace std;
using Matrix = vector<vector<long long>>;
ofstream fout("matrix.out");

const long long MOD = 1e9 + 7;
pthread_barrier_t barrier;

// Structura pentru a transmite argumentele către fiecare thread
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

    // Calculul elementelor din intervalul de rânduri asignat acestui thread
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

    // Barieră pentru sincronizare
    pthread_barrier_wait(&barrier);

    // Copierea rezultatelor din tempResult în result
    for (int i = startRow; i < endRow; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            result[i][j] = tempResult[i][j];
        }
    }

    pthread_exit(nullptr);
}

// Funcția de înmulțire a matricilor folosind Pthreads
void pthreadMatrixMultiply(const Matrix &A, const Matrix &B, Matrix &result, int n, int numThreads)
{
    Matrix tempResult(n, vector<long long>(n, 0));
    vector<pthread_t> threads(numThreads);
    vector<ThreadData> threadData(numThreads);

    // Inițializăm bariera
    pthread_barrier_init(&barrier, nullptr, numThreads);

    // Împărțirea rândurilor între thread-uri
    int rowsPerThread = n / numThreads;
    int remainingRows = n % numThreads;

    for (int i = 0; i < numThreads; ++i)
    {
        int startRow = i * rowsPerThread;
        int endRow = (i == numThreads - 1) ? (startRow + rowsPerThread + remainingRows) : (startRow + rowsPerThread);

        threadData[i] = {&A, &B, &tempResult, &result, startRow, endRow, n};

        // Crearea thread-urilor
        pthread_create(&threads[i], nullptr, threadMatrixMultiplyAndCopy, &threadData[i]);
    }

    // Așteptarea finalizării thread-urilor
    for (int i = 0; i < numThreads; ++i)
    {
        pthread_join(threads[i], nullptr);
    }

    // Distrugem bariera
    pthread_barrier_destroy(&barrier);
}

int main(int argc, char *argv[])
{
    if (argc < 6)
    {
        cerr << "Usage: " << argv[0] << " <input_file> <power> <city_i> <city_j> <num_threads>" << endl;
        return 1;
    }

    string filename = argv[1];
    int k = stoi(argv[2]);
    int city_i = stoi(argv[3]);
    int city_j = stoi(argv[4]);
    int numThreads = stoi(argv[5]);

    assert(numThreads > 0 && (numThreads & (numThreads - 1)) == 0); // Verificăm că numThreads este o putere a lui 2

    int n;
    Matrix adjMatrix, resultMatrix;

    // Citirea matricei din fișier
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

    // Inițializăm matricea rezultat ca matricea identitate
    for (int i = 0; i < n; ++i)
    {
        resultMatrix[i][i] = 1;
    }

    Matrix base = adjMatrix;

    int kk = k;

    // Ridicarea la putere folosind înmulțirea de matrici cu Pthreads
    while (k > 0)
    {
        if (k % 2 == 1)
        {
            pthreadMatrixMultiply(resultMatrix, base, resultMatrix, n, numThreads);
        }
        pthreadMatrixMultiply(base, base, base, n, numThreads);
        k /= 2;
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            fout << resultMatrix[i][j] << " ";
        }
        fout << '\n';
    }

    // Afișăm rezultatul
    cout << "Number of ways of length " << kk << " between city " << city_i << " and city " << city_j << " is: ";
    cout << resultMatrix[city_i][city_j] << endl;

    return 0;
}
