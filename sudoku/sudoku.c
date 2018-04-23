#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
	int row;
	int column;
	int threadNum;
} parameters;

#define maxRow 9
#define maxCol 9

int grid[maxRow][maxCol]  = {{6,5,3,1,2,8,7,9,4},
							 {1,7,4,3,5,9,6,8,2},
							 {9,2,8,4,6,7,5,3,1},
							 {2,8,6,5,1,4,3,7,9},
							 {3,9,1,7,8,2,4,5,6},
							 {5,4,7,6,9,3,2,1,8},
							 {8,6,5,2,3,1,9,4,7},
							 {4,1,2,9,7,5,8,6,3},
							 {7,3,9,8,4,6,1,2,5}};

int checkArray[27] = {0}; //27 threads are going to be created
						  //9 rows, 9 cols, 9 subgrids

void *checkRow(void *data)
{
	parameters *data1 = (parameters *)data;
	checkArray[data1->threadNum] = 1; //assume check succeed first
	int rowCheckArray[9] = {0};
	for(int i = 0;i < maxCol;i++)
	{
		if(rowCheckArray[grid[data1->row][i]-1] != 1)
		{
			rowCheckArray[grid[data1->row][i]-1] = 1;
		}

		else
		{
			checkArray[data1->threadNum] = 0; //check failed
			break;
		}
	}
	free(data); //free the data
}

void *checkColumn(void *data)
{
	parameters *data1 = (parameters *)data;
	checkArray[data1->threadNum] = 1; //assume check succeed first
	int colCheckArray[9] = {0};
	for(int i = 0;i < maxRow;i++)
	{
		if(colCheckArray[grid[i][data1->column]-1] != 1)
		{
			colCheckArray[grid[i][data1->column]-1] = 1;
		}

		else
		{
			checkArray[data1->threadNum] = 0; //check failed
			break;
		}
	}

	free(data); //free the data
}

void *checkSubGrid(void *data)
{
	parameters *data1 = (parameters *)data;
	checkArray[data1->threadNum] = 1; //assume check succeed first
	int gridCheckArray[9] = {0};
	for(int i = data1->row;i<data1->row + 3;i++)
	{
		for(int j = data1->column;j<data1->column+3;j++)
		{
			if(gridCheckArray[grid[i][j]-1] != 1)
			{
				gridCheckArray[grid[i][j]-1] = 1;
			}

			else
			{
				checkArray[data1->threadNum] = 0; //check failed
				break;
			}
		}
	}

	free(data);
}

int main(void)
{
	printf("CS149 Sudoku from Minh An Cao\n");
	pthread_t tidArray[27];
	int threadCounter = 0;//to keep track of the threads

	for(int i = 0;i<maxRow;i++)//rows check
	{
		parameters *data = (parameters *) malloc(sizeof(parameters));
		data->row = i;
		data->column = 0;
		data->threadNum = threadCounter;
		pthread_create(&tidArray[i], NULL, checkRow, (void*) data);
		threadCounter++;
	}


	for(int i = 0;i<maxCol;i++)//columns check
	{
		parameters *data = (parameters *) malloc(sizeof(parameters));
		data->row = 0;
		data->column = i;
		data->threadNum = threadCounter;

		pthread_create(&tidArray[threadCounter], NULL, checkColumn, (void*) data);
		threadCounter++;
	}

	for(int i = 0;i<9;i+=3)//subgrids check
	{
		for(int j=0;j<9;j+=3)
		{
			parameters *data = (parameters *) malloc(sizeof(parameters));
			data->row = i;
			data->column = j;
			data->threadNum = threadCounter;

			pthread_create(&tidArray[threadCounter], NULL, checkSubGrid, (void*) data);
			threadCounter++;
		}
	}

	for(int i = 0;i<27;i++)
	{
		pthread_join(tidArray[i], NULL);
	}

	printf("\n");
	for(int i = 0;i<maxRow;i++)
    {
        for(int j = 0;j<maxCol;j++)
        {
            printf("%d ", grid[i][j]);
            if(j == 2 || j == 5)
            {
                printf("|");
            }
        }
        if(i == 2 || i == 5)
        {
            printf("\n-------------------");
        }
        printf("\n");
    }
    printf("\n");

	int checkIfSuccess = 1;

	for(int i = 0;i<27;i++) //checking each region to see if sudoku is valid
	{
		if(checkArray[i] != 1)
		{
			checkIfSuccess = 0;
			break;
		}
	}

	if(checkIfSuccess == 1)
	{
		printf("Sudoku is valid!\n");
	}

	else
	{
		printf("Sorry, sudoku is invalid!\n");
	}

	return 0;
}

