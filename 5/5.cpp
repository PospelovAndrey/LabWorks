#include <iostream>
#include <mpi.h>
#include <windows.h>
#include <vector>

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

vector<int> ElementsPerProcesses(int col, int size)
{
    vector<int> elementsPerProcesses(size, ((SIZE-1- col) / size) * (SIZE + 1));
    int remaining = (SIZE-1- col) % size;
    for (int i = 0; i < remaining; ++i)
    {
        elementsPerProcesses[i] += (SIZE + 1);
    }
    return elementsPerProcesses;
}

void ForwardRunning(double A[SIZE][SIZE + 1], int rank, int size)
{
    vector<int> sendcounts(size);
    vector<int> displs(size);

    for (int col = 0; col < SIZE - 1; col++)
    {
        sendcounts = ElementsPerProcesses(col, size);
        for (int i = 0; i < size; i++)
        {
            if (i > 0)
                displs[i] = displs[i - 1] + sendcounts[i - 1];
            else
                displs[i] = 0;
        }
        MPI_Bcast(A[col], (SIZE + 1) , MPI_DOUBLE, 0, MPI_COMM_WORLD);
        vector<double>resultRows(sendcounts[rank]);
        MPI_Scatterv(A[col+1], sendcounts.data(), displs.data(), MPI_DOUBLE, resultRows.data(), resultRows.size(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
        for (int row = 0; row < resultRows.size() / (SIZE + 1); row++)
        {
           double ratio = resultRows[row * (SIZE + 1) + col] / A[col][col];
            for (int i = col; i <= SIZE; i++)
            {
                resultRows[row * (SIZE + 1) + i] -= ratio * A[col][i];
            }
        }
        MPI_Gatherv(resultRows.data(), sendcounts[rank], MPI_DOUBLE, A[col + 1], sendcounts.data(), displs.data(), MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
}

void ReverseRunning(double A[SIZE][SIZE + 1], double x[SIZE])
{
    int row, col;
    for (row = (SIZE - 1); row >= 0; row--)
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
