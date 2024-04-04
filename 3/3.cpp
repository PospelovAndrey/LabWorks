#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <windows.h>

using namespace std;

#define SIZE 3000

void printMatrix(const vector<vector<double>>& matrix) {
    for (const auto& row : matrix) {
        for (double elem : row) {
            cout << elem << "\t";
        }
        cout << endl;
    }
}

void ForwardRunning(vector<vector<double>>&A) {
    int row;
    for (int col = 0; col < SIZE - 1; col++) {
#pragma omp parallel for shared(A) private(row)
        for (row = col + 1; row < SIZE; row++) {
            double ratio = A[row][col] / A[col][col];
            for (int i = col; i <= SIZE; i++) {
                A[row][i] -= ratio * A[col][i];
            }
        }
    }
}

void ReverseRunning(const vector<vector<double>>& A, vector<double>& x) {
    for (int row = SIZE - 1; row >= 0; row--) {
        x[row] = A[row][SIZE];
        for (int col = row + 1; col < SIZE; col++) {
            x[row] -= A[row][col] * x[col];
        }
        x[row] /= A[row][row];
    }
}

void generateRandomSystem(vector<vector<double>>& A) {
    for (int i = 0; i < SIZE; ++i) {
        vector<double> row(SIZE + 1);
        int sum = 0;
        for (int j = 0; j <= SIZE; ++j) {
            if (j != SIZE) {
                int num;
                do {
                    num = rand() % 10;
                } while (num == 3 || num == 7 || num == 6 || num == 9);
                row[j] = num;
                sum += num;
            }
            else {
                row[j] = sum;
            }
        }
        A.push_back(row);
    }
}

void writeVectorToFile(const string& filename, const vector<vector<double>>& data) {
    ofstream outFile(filename);
    for (const auto& row : data) {
        for (double elem : row) {
            outFile << elem << " ";
        }
        outFile << endl;
    }
    outFile.close();
}

void readVectorFromFile(const string& filename, vector<vector<double>>& data) {
    ifstream inFile(filename);
    double value;
    vector<double> row;
    while (inFile >> value) {
        row.push_back(value);
        if (row.size() == SIZE + 1) {
            data.push_back(row);
            row.clear();
        }
    }
    inFile.close();
}

int main() {
    const string filename = "array.txt";
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    vector<double> x(SIZE);

    /*vector<vector<double>> A;
    generateRandomSystem(A);
    writeVectorToFile(filename, A);*/
    vector<vector<double>> readArray;
    readVectorFromFile(filename, readArray);

    clock_t start = clock();
    ForwardRunning(readArray);
    ReverseRunning(readArray, x);
    clock_t end = clock();
    double duration = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    cout << "Время выполнения: " << duration * 1000 << " миллисекунд." << endl;

    return 0;
}
