#include <iostream>
#include "mpi.h"
#include <windows.h>
#include <vector>

using namespace std;

#define SIZE 3

void PrintMatrix(int m[SIZE][SIZE])
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            cout << m[i][j] << "\t";
        }
        cout << endl;
    }
}

void GenerateRandomMatrix(int m[SIZE][SIZE])
{
    for (int i = 0; i < SIZE; i++) 
    {
        for (int j = 0; j < SIZE; j++) 
        {
            m[i][j] = rand() % 10;
        }
    }
}

vector<int> ElementsPerProcesses(int size)
{
    vector<int> elementsPerProcesses(size, (SIZE / size)*SIZE);
    int remaining = SIZE % size;
    for (int i = 0; i < remaining; ++i)
    {
        elementsPerProcesses[i] += SIZE;
    }
    return elementsPerProcesses;
}

int main(int argc, char* argv[])
{
    int A[SIZE][SIZE];
    int B[SIZE][SIZE];
    int C[SIZE][SIZE] = { 0 };

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size <= SIZE)
    {
        vector<int> sendcounts(size);
        vector<int> displs(size);
        if (rank == 0)
        {
            GenerateRandomMatrix(A);
            GenerateRandomMatrix(B);
            sendcounts = ElementsPerProcesses(size);           
            for (int i = 0; i < size; i++)
            {
                if (i > 0)
                    displs[i] = displs[i - 1] + sendcounts[i - 1];
                else
                    displs[i] = 0;
            }
        }

        MPI_Bcast(B[0], SIZE * SIZE, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(sendcounts.data(), sendcounts.size(), MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Bcast(displs.data(), displs.size(), MPI_INT, 0, MPI_COMM_WORLD);

        vector<int> rows(sendcounts[rank]);
        MPI_Scatterv(A, sendcounts.data(), displs.data(), MPI_INT, rows.data(), rows.size(), MPI_INT, 0, MPI_COMM_WORLD);
        vector<int>resultRows(rows.size());
        for (int i = 0; i < rows.size() / SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                for (int k = 0; k < SIZE; k++)
                {
                    resultRows[i * SIZE + j] += rows[i * SIZE + k] * B[k][j];
                }
            }
        }
        MPI_Gatherv(resultRows.data(), sendcounts[rank], MPI_INT, C, sendcounts.data(), displs.data(), MPI_INT, 0, MPI_COMM_WORLD);

        if (rank == 0)
        {
            cout << "\nA:\n";
            PrintMatrix(A);
            cout << "\nB:\n";
            PrintMatrix(B);
            cout << "\nC:\n";
            PrintMatrix(C);
        }
        MPI_Finalize();
    }
    else
    {
        if (rank == 0)
        {
            cout << "The number of processes exceeds the dimension of the matrices. Dimension:" << SIZE << " Number of processes:" << size;
        }
        MPI_Finalize();
    }
    return 0;
}
