#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>


int main(int argc, char *argv[]){
/*David Gleaton 9/29/2020
 *
 *Need to Divide the samples by the number of processes in order to split up work
 *How this algorithm works is that there is random sampling in each, 
        and if it falls within [-1,1] the count is incremented.
        So, for this program, all I have to do is divide the amount
        of sampling tasks amongst the process, and have each count.
        After each count is done, the count should be added to the 
        master process count. 
        Then, once all processes have finished, the master count then
        calculates the pi approximation.
*
 *
 * 
*/
 	//Instead of using rank, I'm using pid since that makes it a bit easier for me
	//to track! :)
 	int pid, numprocess;
        int items_per;
        int items_got;


        MPI_Status status;

        MPI_Init(&argc, &argv);


        MPI_Comm_rank(MPI_COMM_WORLD, &pid);
        MPI_Comm_size(MPI_COMM_WORLD, &numprocess);


        int i, count;
        double x, y;
        int samples;
        double pi;
	int slave_count = 0;
        samples = atoi(argv[1]);

	//Calculate how many samples each process needs to calculate
	items_per = samples/numprocess;


	//Master Process
	if(pid != 0){
		slave_count = 0;
		//Get partial count
	        for(i = 0; i < items_per; i++){
	                x = (double) rand() / RAND_MAX;
	                y = (double) rand() / RAND_MAX;
	                if(x*x + y*y <= 1)
	                        slave_count++;
		}
		//Send slave count to master
		printf("I am process %d\n", pid);
		MPI_Send(&slave_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

	}
	//Slaves
	else{
		double starttime, endtime;
		starttime = MPI_Wtime();
		int i;
		//master does its own work to add to count
	        count = 0;
	        for(i = 0; i < items_per; i++){
	                x = (double) rand() / RAND_MAX;
	                y = (double) rand() / RAND_MAX;
	                if(x*x + y*y <= 1)
	                        count++;
	        }
		//Collection of counts from other processes
		int buf;
		for(i = 1; i < numprocess; i++){
			MPI_Recv(&buf, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
			int sender = status.MPI_SOURCE;

			count += buf;
		}
		//Compute Final pi calculation
	        pi = 4.0 * (double)count/(double)samples;
	        printf("Count = %d, Samples = %d, Estimate of pi = %7.5f\n", count, samples, pi);
		endtime = MPI_Wtime();
		printf("Time Elasped is: %f\n", endtime-starttime);
	}

	//Clean up
	MPI_Finalize();



	return 0;

}
