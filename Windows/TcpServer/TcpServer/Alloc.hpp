#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

#include <cstddef>
void* operator new(size_t);
void* operator new[](size_t);
void operator delete(void*);
void operator delete[](void*);
void* mem_alloc(size_t);
void mem_free(void*);

#include <cstdlib>
#include "memorypool.h"

void* operator new(size_t size)
{
	return MemoryManage::getInstance().allocMem(size);
}

void operator delete(void* p)
{
	MemoryManage::getInstance().freeMem(p);
}

void* operator new[](size_t size)
{
	return MemoryManage::getInstance().allocMem(size);
}


void operator delete[](void* p)
{
	MemoryManage::getInstance().freeMem(p);
}

void* mem_alloc(size_t size)
{
	return malloc(size);
}

void mem_free(void* p)
{
	free(p);
}

#endif