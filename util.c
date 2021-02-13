#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include "util.h"

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
		memset((int*)dummy_page, dummy_value, L3CACHE_SIZE/size(int));
	}

	return;
}