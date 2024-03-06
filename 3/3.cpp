#include <iostream>
#include <omp.h>
#include <windows.h>
using namespace std;

#define MAX_SIZE 100

void printMatrix(double matrix[MAX_SIZE][MAX_SIZE + 1], int size) 
{
    for (int i = 0; i < size; i++) 
    {
        for (int j = 0; j <= size; j++) 
        {
            cout << matrix[i][j] << "\t";
        }
        cout << endl;
    }
}

void ForwardRunning(double A[MAX_SIZE][MAX_SIZE + 1], double x[MAX_SIZE], int size)
{
    int row, col;
    for (col = 0; col < size - 1; col++)
    {
#pragma omp parallel for shared(A) private(row)
        for (row = col + 1; row < size; row++)
        {
            if (A[col][col] == 0) {

                int swap_row = -1;
                for (int k = col + 1; k < size; k++)
                {
                    if (A[k][col] != 0)
                    {
                        swap_row = k;
                        break;
                    }
                }
                if (swap_row == -1)
                {
                    cerr << "\nСистема не имеет решений или их кол-во бесконечно" << endl;
                    exit(1);
                }

                for (int k = col; k <= size; k++)
                {
                    double temp = A[col][k];
                    A[col][k] = A[swap_row][k];
                    A[swap_row][k] = temp;
                }
            }

            double ratio = A[row][col] / A[col][col];
            for (int i = col; i <= size; i++) {
                A[row][i] -= ratio * A[col][i];
            }
        }
    }
}

void ReverseRunning(double A[MAX_SIZE][MAX_SIZE + 1], double x[MAX_SIZE], int size) 
{
    int row, col;
    for (row = size - 1; row >= 0; row--) 
    {
        x[row] = A[row][size];
        for (col = row + 1; col < size; col++) 
        {
            x[row] -= A[row][col] * x[col];
        }
        x[row] /= A[row][row];
    }
}

int main() 
{

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int size;
    double A[MAX_SIZE][MAX_SIZE + 1]; 
    double x[MAX_SIZE];                

    cout << "Введите количество уравнений и неизвестных: ";
    cin >> size;

    cout << "Введите коэффициенты уравнений:\n";
    for (int i = 0; i < size; i++) 
    {
        cout << "\nУравнение " << i + 1 << ":\n";
        for (int j = 0; j < size; j++) 
        {
            cout << "Коэффициент для переменной " << j + 1 << ": ";
            cin >> A[i][j];
        }
        cout << "Введите свободный член: ";
        cin >> A[i][size];
    }

    cout << "\nМатрица A до преобразований:\n";
    printMatrix(A, size);

    ForwardRunning(A, x, size);
    ReverseRunning(A, x, size);

    cout << "\nМатрица A после преобразований:\n";
    printMatrix(A, size);

    cout << "\nРешение:\n";
    for (int i = 0; i < size; i++)
    {
        cout << "x"<< i+1 << " = " << x[i] << "\n";
    }

    return 0;
}
