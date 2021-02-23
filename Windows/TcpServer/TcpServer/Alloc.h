#pragma once
#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

void* operator new(size_t);
void* operator new[](size_t);
void operator delete(void*);
void operator delete[](void*);
void* mem_alloc(size_t);
void mem_free(void*);

#endif