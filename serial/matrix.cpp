#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using Matrix = vector<vector<long long>>;

const int MOD = 1e9 + 7;

Matrix multiply(const Matrix &a, const Matrix &b, int n)
{
    Matrix result(n, vector<long long>(n, 0));
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            for (int k = 0; k < n; ++k)
            {
                result[i][j] = (result[i][j] + a[i][k] * b[k][j]) % MOD;
            }
        }
    }
    return result;
}

Matrix matrixExponentiation(Matrix base, int exp, int n)
{
    Matrix result(n, vector<long long>(n, 0));
    for (int i = 0; i < n; ++i)
    {
        result[i][i] = 1;
    }

    while (exp > 0)
    {
        if (exp % 2 == 1)
        {
            result = multiply(result, base, n);
        }
        base = multiply(base, base, n);
        exp /= 2;
    }

    return result;
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        cerr << "Usage: " << argv[0] << " <input_file> <power> <city_i> <city_j>" << endl;
        return 1;
    }

    string filename = argv[1];
    int k = stoi(argv[2]);
    int city_i = stoi(argv[3]);
    int city_j = stoi(argv[4]);

    ifstream infile(filename);
    if (!infile)
    {
        cerr << "Error opening file: " << filename << endl;
        return 1;
    }

    int n;
    infile >> n;

    // validate i and j based on n
    if (city_i < 0 || city_i >= n || city_j < 0 || city_j >= n)
    {
        cerr << "Invalid city index" << endl;
        return 1;
    }

    Matrix adjMatrix(n, vector<long long>(n, 0));
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            infile >> adjMatrix[i][j];
        }
    }
    infile.close();

    Matrix resultMatrix = matrixExponentiation(adjMatrix, k, n);

    cout << "Number of ways of length " << k << " between city " << city_i << " and city " << city_j << " is: ";
    cout << resultMatrix[city_i][city_j] << endl;

    return 0;
}
