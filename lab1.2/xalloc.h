#pragma once

#include <stdlib.h>
#include <stdio.h>


void* xmalloc(size_t size);

void* xcalloc(size_t num, size_t size);

void* xrealloc(void *ptr, size_t size);
