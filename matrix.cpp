#include <iostream>
#include <vector>

using namespace std;
using Matrix = vector<vector<long long>>;

const int MOD = 1e9 + 7;  // Modulo pentru a evita overflow-ul (dacă este necesar).

// Funcție pentru înmulțirea a două matrici
Matrix multiply(const Matrix &a, const Matrix &b, int n) {
    Matrix result(n, vector<long long>(n, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                result[i][j] = (result[i][j] + a[i][k] * b[k][j]) % MOD;
            }
        }
    }
    return result;
}

// Funcție pentru exponențierea rapidă a unei matrici
Matrix matrixExponentiation(Matrix base, int exp, int n) {
    Matrix result(n, vector<long long>(n, 0));
    for (int i = 0; i < n; ++i) {
        result[i][i] = 1;  // Matricea identitate
    }

    while (exp > 0) {
        if (exp % 2 == 1) {
            result = multiply(result, base, n);
        }
        base = multiply(base, base, n);
        exp /= 2;
    }

    return result;
}

int main() {
    int n, m, k;
    cout << "Introduceți numărul de noduri (n), numărul de legături (m) și lungimea drumului (k): ";
    cin >> n >> m >> k;

    // Inițializăm matricea de adiacență
    Matrix adjMatrix(n, vector<long long>(n, 0));

    cout << "Introduceți fiecare legătură (nod1 nod2):" << endl;
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        adjMatrix[u][v] = 1;
        adjMatrix[v][u] = 1;  // Graful este neorientat
    }

    // Ridicăm matricea de adiacență la puterea k
    Matrix resultMatrix = matrixExponentiation(adjMatrix, k, n);

    cout << "Matricea de adiacență ridicată la puterea " << k << " este:" << endl;
    for (const auto &row : resultMatrix) {
        for (const auto &value : row) {
            cout << value << " ";
        }
        cout << endl;
    }

    return 0;
}
