/* C Example */
#include <stdio.h>
#include <mpi.h>


 
 MPI_Status status;
 int ROWS_IN_ORIGINAL_ARRAY=1200;
 int COLS_IN_ORIGINAL_ARRAY=1024;
 
 
  

int main (int argc, char **argv)
{
int rank, size;
MPI_Init (&argc, &argv); /* starts MPI */

 MPI_Comm_rank (MPI_COMM_WORLD, &rank); /* get current process id */
 MPI_Comm_size (MPI_COMM_WORLD, &size); /* get number of processes */



if(rank == 0)//Master Process code starts
{

//Initializing the Array in the Master Process
int originalArray[ROWS_IN_ORIGINAL_ARRAY][COLS_IN_ORIGINAL_ARRAY];  //Initializing the original Array
//Poppulating the original Array in the Master Process
//Row i will contain i for all the columns
//For ex- Row 2 will have  2 in all the columns
int i,j=0;
for( i=0;i<ROWS_IN_ORIGINAL_ARRAY;i++)
{
    for( j=0;j<COLS_IN_ORIGINAL_ARRAY;j++)
    {
	originalArray[i][j]=i;
	}//end of inner loop
}//end of outer loop

	
int numberOfTasksPerProcess=ROWS_IN_ORIGINAL_ARRAY/size;  	

//The following array will contain the starting index(s) of the respective chunks(of the original array) assigned to various slave processes 
int assignmentStartingIndex[size];
	int counter=0;
	
	for(j=0;j<ROWS_IN_ORIGINAL_ARRAY;j++)
	{
		if(j%numberOfTasksPerProcess == 0)
		{
		assignmentStartingIndex[counter]=j;//Computing and saving the starting index of chunks assigned to slave processes
		counter=counter+1;
		}
	}//end of loop


    //Sending array chunks to slave processes	
	for(j=1;j<size;j++)   //Since ranks of first and last slave processes are 1 and (size-1)
	{
		int startIndex=assignmentStartingIndex[j];
		int endIndex; 
		//In case the number of rows is not perfectly divisible by the number of processes
		if(j==size-1) {endIndex=ROWS_IN_ORIGINAL_ARRAY-1;}
		else{endIndex=startIndex + numberOfTasksPerProcess-1;}
		

        MPI_Request requestArray[endIndex-startIndex+1];
        int counter2=0;
        //sending rows from startIndex to endIndex one by one
		int l=0;
		for( l=startIndex;l<=endIndex;l++)
		{
		MPI_Isend(&originalArray[l],COLS_IN_ORIGINAL_ARRAY,MPI_INT,j,l,MPI_COMM_WORLD,&requestArray[counter2]);
        counter2=counter2+1;
		}
		
	
	} //Sending data to slave processes completed


	//Calculation of Average by Master Process
	int m,n=0;
	int sum=0;
	for(m=0;m<= ((ROWS_IN_ORIGINAL_ARRAY/size)-1)  ;m++)
	{
		for(n=0;n<COLS_IN_ORIGINAL_ARRAY;n++)
		{
			sum=sum+originalArray[m][n];
		}//Inner loop ends
		printf("\nAverage(from process %d) for row  %d is %d ",rank,m,(sum/COLS_IN_ORIGINAL_ARRAY));
		sum=0;
	}//outer loop ends

	
	
	
} // End of Master code


//Slave code starts
else
{
	int numberOfTasksPerProcess=ROWS_IN_ORIGINAL_ARRAY/size;  	
	int assignmentStartingIndex[size];  //Initializing the Array which will have the starting Index of the chunk for all slave processes	
	int counter=0;
	int j=0;
	for(j=0;j<ROWS_IN_ORIGINAL_ARRAY;j++)
	{
		if(j%numberOfTasksPerProcess == 0)
		{
		assignmentStartingIndex[counter]=j;
		counter=counter+1;	
		}
	}//end of loop

	
    int startIndex=	assignmentStartingIndex[rank];
	int endIndex;
		//In case the number of rows is not perfectly divisible by the number of processes
		if(rank==size-1) {endIndex=ROWS_IN_ORIGINAL_ARRAY-1;}
			else{endIndex=startIndex + numberOfTasksPerProcess-1;}
		
		int numberOfRowsToCalculateAverageOf= endIndex-startIndex+1;

		int temporaryArray[numberOfRowsToCalculateAverageOf][COLS_IN_ORIGINAL_ARRAY];  //Initializing the temporary Array which holds the chunk


	MPI_Status statusArray[numberOfRowsToCalculateAverageOf];
	MPI_Request requestArray[numberOfRowsToCalculateAverageOf];
	int done[numberOfRowsToCalculateAverageOf];
	
	
	int i;
	int averageCalculatedFlagArray[numberOfRowsToCalculateAverageOf]; //all elements 0
	for(i=0;i<numberOfRowsToCalculateAverageOf;i++)
	{
		averageCalculatedFlagArray[i]=0;
	}
			
    //Code for receiving Data starts
	 counter=0;
	
	for(i=startIndex;i<=endIndex;i++)
	{
	MPI_Irecv(&temporaryArray[counter],COLS_IN_ORIGINAL_ARRAY,MPI_INT,0,i,MPI_COMM_WORLD,&requestArray[i-startIndex]);	
	counter=counter+1;
	}//end of loop
	
	
	int arrayHavingAverages[numberOfRowsToCalculateAverageOf]; //This array will finally have the average of the rows

	
	
	int noOfAveragesCalculated=0;
    
	// Parallel Calculation starts

	/*
	Following while loop parallely calculates the average of different rows.
	Variable numberOfRowsToCalculateAverageOf contains the number of rows for which average has to be calculated.
	Variable noOfAveragesCalculated holds the number of averages which have been calculated so far.
	Array averageCalculatedFlagArray will have 0 for the indexes for which average has not been calculated.
	And it will contain -99 in the respective index for the row of which , average has been calculated.
	
	while(number of averages calculated is not equal to the number of rows in the chunk)
	{
		for (iterate i times where i is the number of rows in the chunk)
		{
         Test for the ith row if it has been successfully received.
			if(row is received && average has not been calculated before)
			{
				Calculate Average;
				Increment respective counters and set respective flags;
			}
		
		}
	}
	
	*/
	
int sum=0;
while(noOfAveragesCalculated != numberOfRowsToCalculateAverageOf)
{
	
	for(i=0;i<numberOfRowsToCalculateAverageOf;i++)
	{
		MPI_Test(&requestArray[i],&done[i],&statusArray[i]);
		if(done[i]  &&  (averageCalculatedFlagArray[i] != -99))
		{
		averageCalculatedFlagArray[i]=-99;
		noOfAveragesCalculated=noOfAveragesCalculated+1;
		for(j=0;j<COLS_IN_ORIGINAL_ARRAY;j++)
		{
			sum=sum+temporaryArray[i][j];
        }//end of inner for 

		arrayHavingAverages[i]=(sum/COLS_IN_ORIGINAL_ARRAY);
        sum=0;
			
		}	//end of if
	}//end of outer  for
	
}//end of while

//Finally printing the average
for(i=0;i<numberOfRowsToCalculateAverageOf;i++)
{
printf("\nAverage(from process %d) for row  %d is %d ",rank,(i+startIndex),arrayHavingAverages[i]);	
}

//Parallel Calculation ends
	
}//Slave code ends

MPI_Finalize();
 return 0;
}
