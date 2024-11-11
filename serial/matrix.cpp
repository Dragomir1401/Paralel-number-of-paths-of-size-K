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
    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << " <input_file> <power>" << endl;
        return 1;
    }

    string filename = argv[1];
    int k = stoi(argv[2]);

    ifstream infile(filename);
    if (!infile)
    {
        cerr << "Error opening file: " << filename << endl;
        return 1;
    }

    int n;
    infile >> n;

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

    cout << "Matricea de adiacență ridicată la puterea " << k << " este:" << endl;
    for (const auto &row : resultMatrix)
    {
        for (const auto &value : row)
        {
            cout << value << " ";
        }
        cout << endl;
    }

    return 0;
}
