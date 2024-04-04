#include <iostream>
#include "mpi.h"

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

int main(int argc, char* argv[])
{
    int A[SIZE][SIZE];
    int B[SIZE][SIZE];
    int C[SIZE][SIZE] = { 0 };
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (rank == 0) 
    {
        GenerateRandomMatrix(A);
        GenerateRandomMatrix(B);
    }
    int row[SIZE];
    int resultRow[SIZE] = { 0 };
    MPI_Scatter(A[0], SIZE, MPI_INT, row, SIZE, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(B[0], SIZE * SIZE, MPI_INT, 0, MPI_COMM_WORLD);
    for (int i = 0; i < SIZE; i++) 
    {
        for (int j = 0; j < SIZE; j++) 
        {
            resultRow[i] += row[j] * B[j][i];
        }     
    }
    MPI_Gather(resultRow, SIZE, MPI_INT, C[0], SIZE, MPI_INT, 0, MPI_COMM_WORLD);
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
    return 0;
}
