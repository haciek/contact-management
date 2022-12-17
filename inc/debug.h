#pragma once
#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>

extern void* debug_malloc(size_t size, char* file, uint line);
extern void debug_free(void* buf, char* file, uint line);

#endif // !DEBUG
