#include <iostream>
#include <mpi.h>
#include <windows.h>

using namespace std;

#define SIZE 3

void PrintMatrix(double matrix[SIZE][SIZE + 1])
{
    cout << endl;
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j <= SIZE; j++)
        {
            cout << matrix[i][j] << "\t";
        }
        cout << endl;
    }
}

void ForwardRunning(double A[SIZE][SIZE + 1], int rank, int size)
{
    MPI_Status status;
    for (int col = 0; col < SIZE - 1; col++)
    {
        MPI_Bcast(A, SIZE * (SIZE + 1), MPI_DOUBLE, 0, MPI_COMM_WORLD);
        for (int row = col + 1 + rank; row < SIZE; row += size)
        {
            double ratio = A[row][col] / A[col][col];
            for (int i = col; i <= SIZE; i++)
            {
                A[row][i] -= ratio * A[col][i];
            }
        }
        if (rank != 0) 
        {
            for (int row = col + 1 + rank; row < SIZE; row += size) 
            {
                MPI_Send(&A[row][0], SIZE + 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
            }
        }
        else 
        {
            for (int r = 1; r < size; r++) 
            {
                for (int row = col + 1 + r; row < SIZE; row += size) 
                {
                    MPI_Recv(&A[row][0], SIZE + 1, MPI_DOUBLE, r, 0, MPI_COMM_WORLD, &status);
                }
            }

        }
    }
}

void ReverseRunning(double A[SIZE][SIZE + 1], double x[SIZE])
{
    int row, col;
    for (row = SIZE - 1; row >= 0; row--)
    {
        x[row] = A[row][SIZE];
        for (col = row + 1; col < SIZE; col++)
        {
            x[row] -= A[row][col] * x[col];
        }
        x[row] /= A[row][row];
    }
}

int main(int argc, char* argv[])
{
    double A[SIZE][SIZE + 1] = { {2,-1,0,0},{-1,1,4,13},{1,2,3,14}};
    double X[SIZE];
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0)
    {
        cout << "\nA before:\n";
        PrintMatrix(A);
    }
    
    ForwardRunning(A, rank, size);

    if (rank == 0)
    {
        ReverseRunning(A, X);
        cout << "\nA after:\n";
        PrintMatrix(A);

        cout << "\nX:\n";
        for (int i = 0; i < SIZE; i++)
        {
            cout << "x" << i + 1 << " = " << X[i] << "\n";
        }
    }

    MPI_Finalize();
    return 0;
}
