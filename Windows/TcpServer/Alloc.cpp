#include "Alloc.h"
#include <cstdlib>
#include "memorypool.h"

void *operator new(size_t size)
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

