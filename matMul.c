#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

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

void matMul(float A[SIZE][SIZE], float B[SIZE][SIZE], float C[SIZE][SIZE]){
    int i, j, k;
    clock_t t1, t2;

    //CALCULATION IN SERIAL
    t1 = clock();
    for ( i = 0; i < SIZE; i++){
        for ( j = 0; j < SIZE; j++){
            for ( k = 0; k < SIZE; k++){
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    t2 = clock();
    printf("\nSequential time: %12.4g, with SIZE = %d\n", (t2-t1)/1000000.0, SIZE);
    isMatMulCorrect(A,B,C);
    //UNCOMMENT THIS SECTION IN ORDER TO PRINT RESULTS:
    /*
        printMatrix(A);
        printf("\n\n");
        printMatrix(B);
        printf("\n\n");
        printMatrix(C);
    */
}


int main(){
    srand(time(NULL)); 

    float A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
    fillMatrix(A);
    fillMatrix(B);

    matMul(A, B, C);
}