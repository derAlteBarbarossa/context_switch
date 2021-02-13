#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <x86intrin.h>

#define L3CACHE_SIZE	6291456
#define ROUNDS			10000

int compare_ints(const void* a, const void* b)
{
    int arg1 = *(const int*)a;
    int arg2 = *(const int*)b;
 
    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
 
}

int find_median(int array[ROUNDS])
{
	qsort(array, ROUNDS, sizeof(int), compare_ints);
	return array[ROUNDS>>1];
}

void flush_cache()
{
	int dummy_value = rand();
	void* dummy_page = mmap(NULL, L3CACHE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (dummy_page == MAP_FAILED)
	{
		printf("Could not allocate page to flush the cache\n");
		exit(-1);
	}
	else
	{
		memset((int*)dummy_page, dummy_value, L3CACHE_SIZE/sizeof(int));
	}

	return;
}


void measure_single_process(register int file_des[2])
{
	volatile int dummy = 1;
	unsigned int aux = 0;
	uint64_t start;
	int times[ROUNDS] = {0};

	int n;
	char buffer[1025];
    //char *message = "Hello, World!";

    for (int i=0; i < ROUNDS; i++)
    {
    	start = __rdtscp(&aux);
		
    	write(file_des[1], "c", 1);
	    read(file_des[0], buffer, 1);


	    times[i] = __rdtscp(&aux) - start;
    }
    printf("Time elapsed for read-wite: %d\n", find_median(times));
    
	return;
}

void measure_double_process(int file_des_1[2], int file_des_2[2], pid_t p, char* msg)
{
	if(p == 0)
	{

		for (int i=0; i < ROUNDS; i++)
	    {
	    	flush_cache();
	    	write(file_des_1[1], "d", 1);
	    	read(file_des_2[0], msg, 1);    
	    }
	    exit(0);
	}

	else
	{
		uint64_t start;
		unsigned int aux = 0;
		int times[ROUNDS] = {0};

		for (int i=0; i < ROUNDS; i++)
	    {
	    	
	    	start = __rdtscp(&aux);
	    	write(file_des_2[1], "e", 1);
		    read(file_des_1[0], msg, 1);
		    times[i] = __rdtscp(&aux) - start;
	    }	
	    fflush(stdout);
	    printf("Time elapsed for 2*read-wite + 2*context-switch: %d\n", find_median(times));
	}
	return;
}
int main(int argc, char** argv)
{
	char message;
	int pipe_fd1[2], pipe_fd2[2];
	pid_t p;
	p = fork();
	
	if(p < 0)
	{
		perror("fork");
		return -1;
	}
	else
	{
		if(p == 0)
		{
			if(pipe(pipe_fd1) == -1)
			{
				printf("Could not create pipe!\n");
				exit(-1);
			}
			else
			{
				measure_single_process(pipe_fd1);
				exit(0);
			}
		}
	}

	wait(NULL);
	
	if(pipe(pipe_fd1) == -1 || pipe(pipe_fd2) == -1)
	{
		printf("Could not create pipe!\n");
		exit(-1);
	}

	p = fork();
	if(p < 0)
	{
		perror("fork");
		return -1;
	}
	else
	{
		if(p == 0)
		{
			measure_double_process(pipe_fd1, pipe_fd2, p, &message);
			exit(0);
		}

		else
		{
			measure_double_process(pipe_fd1, pipe_fd2, p, &message);
		}
	}


	return 0;
}