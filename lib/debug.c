#include "debug.h"

extern void* debug_malloc(size_t size, char* file, uint line)
{
	void* ptr = malloc(size);
	if (ptr == NULL)
	{

		#ifdef MEM_DEBUG
		printf("\n Malloc in %s on line %d -> nullptr", __FILE__, __LINE__);
		#endif
		exit(-1);
	}
	return ptr;
}

extern void debug_free(void* buf, char* file, uint line)
{
	#ifdef MEM_DEBUG
	printf("\nFreed ptr - %p in %s on line %d\n", buf, file, line);
	#endif /* MEM_DEBUG */
	free(buf);
}
