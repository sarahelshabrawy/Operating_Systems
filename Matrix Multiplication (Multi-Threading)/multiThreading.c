#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct matrix
{
    int **matrix;
    int numberOfRows;
    int numberOfColumns;
} matrix;
typedef struct thread_dataA
{
    matrix *result;
    matrix b;
    int index;
    int *row;
} dataA;
typedef struct thread_dataB
{
    int numberOfColumns;
    int *row;
    int *column;
} dataB;


void singleThreaded(matrix a,matrix b);
void *multiplyRow(void *threadArgs);
void multiThreadedA(matrix a,matrix b);
//void multiThreadedA(int **a, int **b, int rows, int columns, int common);
matrix readMatrix(char* filePath);
void printMatrix(matrix Matrix, char* matrixName);


int main()
{
    
    matrix a = readMatrix("test1/a.txt");
    printMatrix(a,"A");
    matrix b = readMatrix("test1/b.txt");
    printMatrix(b,"B");

   singleThreaded(a, b);
   multiThreadedA(a,b);
    return 0;
}

void singleThreaded(matrix a,matrix b)
{
    struct timeval stop, start;
    matrix result;
    result.numberOfRows = a.numberOfColumns;
    result.numberOfColumns = b.numberOfColumns;
    gettimeofday(&start, NULL); //start checking time
    int** Matrix = malloc(result.numberOfRows*sizeof(*Matrix));
    for (int i = 0; i < result.numberOfRows; i++)
    {
        Matrix[i] = malloc(sizeof *Matrix[i] * b.numberOfColumns);
        for (int j = 0; j < result.numberOfColumns; j++)
        {
            Matrix[i][j] = 0;
            for (int k = 0; k < a.numberOfColumns; k++)
            {
                
                Matrix[i][j] += (a.matrix[i][k] * b.matrix[k][j]);
            }
        }
    }
    result.matrix = Matrix;
    printMatrix(result,"Result");
    gettimeofday(&stop, NULL); //end checking time

    printf("\nSeconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
}

void multiThreadedA(matrix a,matrix b){
      struct timeval stop, start;
    matrix result;
    result.numberOfRows = a.numberOfColumns;
    result.numberOfColumns = b.numberOfColumns;
    pthread_t threads[result.numberOfRows]; /* declare an array of type pthread_t*/
    int rc;
    int** Matrix = malloc(result.numberOfRows*sizeof(*Matrix));
    gettimeofday(&start, NULL); //start checking time
    dataA *data;
    data->result = &result;
    data->b = b;
 
    for(int i=0; i<result.numberOfRows; i++){
        Matrix[i] = malloc(sizeof *Matrix[i] * b.numberOfColumns);
         data->index = i;
         data->row = b.matrix[i];
        rc = pthread_create(&threads[i], NULL, multiplyRow, (void *)data);
        if (rc){
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
    }
    }
    result.matrix = Matrix;
    printMatrix(result,"Result multi");
    gettimeofday(&stop, NULL); //end checking time

    printf("\nSeconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    pthread_exit(NULL); /* the main should also terminate*/
}
matrix readMatrix(char* filePath){
FILE *fp;

    fp = fopen(filePath, "r"); // read mode

    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    int rowsOne;
    int columnsOne;

    fscanf(fp, "row=%d col=%d\n", &rowsOne, &columnsOne);
    int** a = malloc(rowsOne*sizeof(*a));

    for (int i = 0; i < rowsOne; i++)
    {
        a[i] = malloc(sizeof *a[i] * columnsOne);
        for (int j = 0; j < columnsOne; j++)
        {
            fscanf(fp, "%d", &a[i][j]);
        }
    }
    
    matrix Matrix;
    Matrix.matrix = a;
    Matrix.numberOfRows = rowsOne;
    Matrix.numberOfColumns = columnsOne;
    fclose(fp);
    return Matrix;
}

void printMatrix(matrix Matrix, char* matrixName){
     printf("\nMatrix %s \n",matrixName);
    for (int i = 0; i < Matrix.numberOfRows; i++)
    {
        for (int j = 0; j < Matrix.numberOfColumns; j++)
        {
            printf("%d ", Matrix.matrix[i][j]);
        }
        printf("\n");
    }
}

void *multiplyRow(void *threadArgs){
    dataA *data = (dataA *) threadArgs;
    matrix b = data->b;
    matrix* result = data->result;
    for (int j = 0; j < result->numberOfColumns; j++)
        {
            result->matrix[data->index][j] = 0;
            for (int k = 0; k < b.numberOfRows; k++)
            {
                result->matrix[data->index][j] += (data->row[k] * b.matrix[k][j]);
            }
        }
}