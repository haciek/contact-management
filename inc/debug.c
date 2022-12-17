#include "debug.h"

extern void* debug_malloc(size_t size, char* file, uint line)
{
	void* ptr = malloc(size);
#ifdef MEM_DEBUG
	if (ptr == NULL)
	{

		printf("\n Malloc in %s on line %d -> nullptr", __FILE__, __LINE__);
		exit(-1);
	}
#endif
	return ptr;
}

extern void debug_free(void* buf, char* file, uint line)
{
#ifdef MEM_DEBUG
	if (buf == NULL)
	{
		printf("\nNullptr - in %s on line %d\n", file, line);
		exit(-1);
	}
	else
	{
		printf("\nFreeing a ptr - %p in %s on line %d\n", buf, file, line);
	}
#endif /* MEM_DEBUG */
	free(buf);
}
