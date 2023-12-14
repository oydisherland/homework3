# Homework 3

## Description Of The Code

The files matMul.c, matMulPar1.c, matMulPar2.c and matMulBlockPar.c all have different codes that preforms a matrix multiplications. 
Their functionality is similar, they perform a multiplication and print the calculation time when finished. In addition the also print CORRECT or NOT CORRECT, dependent on whether the claculation performed was correct. By unncommenting the sections at the end of the functions marked as: "UNCOMMENT THIS SECTION IN ORDER TO PRINT RESULTS", the matrices A, B and C is also printed. 

At the top of file, line 7 (line and 8 for matMulBlockPar.c), the size of the matrix is defined (block size also for matMulBlackPar.c). In order to run the code with a different matrix-size, these two variables can be edited. NB: the two functions ore only compatible with a sertain compination of SIZE, BLOCKSIZE and number of processors. The functions will exit with an error message if running with a non-compatible combination. 

## How To Run The Code 

In order to run the files on the cluster you only need to edit one line in the matMul.pbs file:
- line 22, marked as "Select the working directory". Here you write the directory all the .c files on the cluster. 

When submittig the matMul.pbs file on the cluster, all the .c files will run. In order to run one file with x of processors, add this line in the pbs script after the file is compiled:
- mpiexec -np x ./outputfile.out

When this is done you can write on the cluster: 
- qsub matMul.pbs
After the calculations there will be made a file called parallelMul.o, this is where the results are printed. 

