#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>

using namespace std;
using Matrix = vector<vector<long long>>;

const int MOD = 1e9 + 7;

// Function to multiply a portion of the matrix
void localMultiply(const Matrix &a, const Matrix &b, Matrix &result, int startRow, int endRow, int n)
{
    for (int i = startRow; i < endRow; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            for (int k = 0; k < n; ++k)
            {
                result[i][j] = (result[i][j] + a[i][k] * b[k][j]) % MOD;
            }
        }
    }
}

int main(int argc, char *argv[])
{
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

    // Broadcast size of the matrix
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Broadcast matrix B to all processes
    if (rank != 0)
    {
        adjMatrix = Matrix(n, vector<long long>(n));
    }
    for (int i = 0; i < n; ++i)
    {
        MPI_Bcast(adjMatrix[i].data(), n, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
    }

    // Determine the rows assigned to this process
    int rowsPerProcess = n / size;
    int startRow = rank * rowsPerProcess;
    int endRow = (rank == size - 1) ? n : startRow + rowsPerProcess;

    // Local buffer for storing partial results for this process
    Matrix localResult(n, vector<long long>(n, 0));
    localMultiply(adjMatrix, adjMatrix, localResult, startRow, endRow, n);

    // Buffer on the root process to gather all results
    Matrix gatherBuffer;
    if (rank == 0)
    {
        gatherBuffer = Matrix(n, vector<long long>(n, 0));
    }

    // Gather results at root process
    for (int i = startRow; i < endRow; ++i)
    {
        MPI_Gather(localResult[i].data(), n, MPI_LONG_LONG,
                   (rank == 0 ? gatherBuffer[i].data() : nullptr),
                   n, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
    }

    if (rank == 0)
    {
        // Copy gathered results into the result matrix
        for (int i = startRow; i < endRow; ++i)
        {
            resultMatrix[i] = gatherBuffer[i];
        }

        cout << "Number of ways of length " << k << " between city " << city_i << " and city " << city_j << " is: ";
        cout << resultMatrix[city_i][city_j] << endl;
    }

    MPI_Finalize();
    return 0;
}
