
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define SIZE 2048
#define BLOCKSIZE 256

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
        printf("NOT CORRECT     %d mistakes found", wrong);
    }
}

//MATRIX MULTIPLICATION BLOCKWISE USING MPI
void matMulBlockPar(float A[SIZE][SIZE], float B[SIZE][SIZE], float C[SIZE][SIZE], int argc, char *argv[]){

    //THE BLOCKSIZE MUST BE CHOOSEN SO SIZE*SIZE MATRIX CAN BE DIVIDED INTO BLOCK*BLOCK MATRIX
    if(SIZE%BLOCKSIZE != 0){
        printf("\nBOCKSIZE must be dvisible with SIZE\n");
        exit(0);
    }

    int i, j, k, w, source, rank, num_procs;
    int numberOfBlocks = (SIZE/BLOCKSIZE)*(SIZE/BLOCKSIZE);
    int blocksPerRow = SIZE/BLOCKSIZE;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs);

    //EXIT CODE IF NUMBER OF BLOCKS ARE NOT COMPATIBLE WITH NUMBER OF PROCESSORS
    if(numberOfBlocks%(num_procs) != 0){
        if(rank==0){
        printf("\nNum of blocks must be compatible with number of procs, NoB = %d, NoP = %d\n", numberOfBlocks, num_procs);
        printf("NumberOfBlocks%(numberOfProcessors) = 0\n");
        }
        MPI_Finalize();
        exit(0);
    }

    //THE NUMBER OF BLOCKS WILL BE DIVIDED EAQUALLY BETWEEN THE PROCESSORS
    int blocksPerProcs = numberOfBlocks/(num_procs);
    
    double w1, w2;
    float calculatedBlocks_send[BLOCKSIZE*BLOCKSIZE*blocksPerProcs];
    float calculatedBlocks_recv[BLOCKSIZE*BLOCKSIZE*blocksPerProcs];
    
    w1 = MPI_Wtime();

    //CALCULATION:
    float sum; 
    for(w=0; w < blocksPerProcs; w++){
        int blocknumber = rank*blocksPerProcs+w;
        for(i = 0; i < BLOCKSIZE; i++){
            for(j = 0; j < BLOCKSIZE; j++){
                sum = 0;
                for(k = 0; k < SIZE; k++){
                    sum += A[i + blocknumber/blocksPerRow * BLOCKSIZE][k] * B[k][j + blocknumber%blocksPerRow * BLOCKSIZE];
                }
                calculatedBlocks_send[j + i*BLOCKSIZE + w*BLOCKSIZE*BLOCKSIZE] = sum;
            }
        }
    }

    if(rank != 0){
      //SEND THE CALCULATED BLOCKS TO PROCESSOR 0
      MPI_Send(calculatedBlocks_send, BLOCKSIZE*BLOCKSIZE*blocksPerProcs, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    }

    if(rank==0){
        //RANK 0 START FILLING IN THE RETURN MATRIX WITH THE BLOCKS IT CALCULATED ITSELF
        for(w = 0; w < blocksPerProcs; w++){
            int blockNumber = w;
            for(j = 0; j < BLOCKSIZE; j++){
                for(k = 0; k<BLOCKSIZE; k++){
                    C[blockNumber/blocksPerRow * BLOCKSIZE + j][blockNumber%blocksPerRow *BLOCKSIZE + k] 
                    = calculatedBlocks_send[k + j*BLOCKSIZE + w*BLOCKSIZE*BLOCKSIZE];
                }
            }
        }
    
        //RESCIEV CALCULATED BLOCKS FROM THE OTHER PROCESSORS
        for(source = 1; source < num_procs; source++){
        
            MPI_Recv(calculatedBlocks_recv, BLOCKSIZE*BLOCKSIZE*blocksPerProcs, MPI_FLOAT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //PUT THE RESCIEVED BLOCKS IN MATRIX C
            for(w = 0; w < blocksPerProcs; w++){
                int blockNumber = source*blocksPerProcs + w;
                for(i = 0; i < BLOCKSIZE; i++){
                    for(j = 0; j<BLOCKSIZE; j++){
                        C[i + blockNumber/blocksPerRow * BLOCKSIZE][j + blockNumber%blocksPerRow *BLOCKSIZE] 
                        = calculatedBlocks_recv[j + i*BLOCKSIZE + w*BLOCKSIZE*BLOCKSIZE];
                    }
                }
            }
        }

        w2 = MPI_Wtime();
        //UNCOMMENT THIS SECTION IN ORDER TO PRINT RESULTS:
    /*
        printMatrix(A);
        printf("\n\n");
        printMatrix(B);
        printf("\n\n");
        printf("I am rank %d and here is C:\n", rank);
        printMatrix(C);
    */
        printf("SIZE = %d and BLOCKSIZE = %d, ", SIZE, BLOCKSIZE);
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

  matMulBlockPar(A,B,C,argc,argv);

}
