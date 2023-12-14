
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define SIZE 2048

void fillMatrix(float m[SIZE][SIZE]){
    int n=0;
    int i, j;
    for (i=0; i<SIZE; i++){
        for (j=0; j<SIZE; j++){
            m[i][j] = rand()%10;
        }
    } 
}

void printMatrix(float m[SIZE][SIZE])
{
    int i, j = 0;
    for (i=0; i<SIZE; i++) {
        printf("\n\t| ");
        for (j=0; j<SIZE; j++){
            printf("%2f ", m[i][j]);
            printf("|");
        }
    }
}

//CHECKING RANDOM VALUES IN C IF THEY ARE CORRECT 
void isMatMulCorrect(float A[SIZE][SIZE], float B[SIZE][SIZE], float C[SIZE][SIZE]){
  int i, j, k;
  int wrong = 0;

  for(i = 0; i < SIZE*2; i++){
    int num1 = rand()%SIZE;
    int num2 = rand()%SIZE;
    float sum = 0;
    for(j = 0; j < SIZE; j++){
      sum += A[num1][j]*B[j][num2];
    }
    if(sum != C[num1][num2]){
      wrong++;
    }
  }
  if(wrong ==0){
    printf("CORRECT\n");
  }else{
    printf("NOT CORRECT     %d mistakes found\n", wrong);
  }
}

//MATRIX MULTIPLICATION USING MPI
void matMulPar(float A[SIZE][SIZE], float B[SIZE][SIZE], float C[SIZE][SIZE], int argc, char *argv[]){
    int i, j, k, rank, num_procs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);


    //ONLY RUN THE CALCULATION IF THE ROWS IN THE MATRIX CAN BE EQUALLY DIVIDED BETWEEN THE PROCESSORS
    if(SIZE%num_procs != 0){
        if(rank==0){
            printf("\nNumber of processors must be divisible with SIZE\n");
        }
        MPI_Finalize();
        exit(0);
    }

    double w1, w2;
    int rowsPerProcs = SIZE/num_procs;
    float calculatedRows[SIZE*rowsPerProcs];

    w1 = MPI_Wtime();
    //CALCULATION:
    for(k=0; k< rowsPerProcs; k++){
        for(i = 0; i < SIZE; i++){
            float sum = 0;
            for(j = 0; j < SIZE; j++){
                sum += A[rank*rowsPerProcs+k][j]*B[j][i];
            }
            calculatedRows[i + k*SIZE] = sum;
        }
    }

    //SENDING RESULTS TO PROCESSOR 0:
    MPI_Gather (calculatedRows, SIZE*rowsPerProcs, MPI_FLOAT, C, SIZE*rowsPerProcs, MPI_FLOAT, 0, MPI_COMM_WORLD);

    w2 = MPI_Wtime();
    if(rank==0){

    //UNCOMMENT THIS SECTION IN ORDER TO PRINT RESULTS:
    /*
        printMatrix(A);
        printf("\n\n");
        printMatrix(B);
        printf("\n\n");
        printf("I am rank %d and here is C:\n", rank);
        printMatrix(C);
    */
        printf("SIZE = %d, ", SIZE);
        printf("time with %d num of procs is:  %12.4g sec\n", num_procs, w2-w1);
        isMatMulCorrect(A,B,C);
    }
    MPI_Finalize();
}


int main(int argc, char *argv[])
{
  srand(time(NULL)); 

  float A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
  fillMatrix(A);
  fillMatrix(B);

  matMulPar(A,B,C,argc, argv);

}
