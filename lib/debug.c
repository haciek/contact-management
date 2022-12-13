#include "debug.h"

void* debug_malloc(size_t size, char* file, uint line)
{
	void* ptr = malloc(size);

	if (ptr == NULL)
	{
		printf("%s: %d line -> nullptr, not enough memory", __FILE__, __LINE__);
		exit(-1);
	}
	return ptr;
}
