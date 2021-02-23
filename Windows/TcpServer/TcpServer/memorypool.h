#pragma once
#ifndef _MEMORYPOOL_H_
#define _MEMORYPOOL_H_
#include <cstdlib>
#include <mutex>
#include <iostream>

const int MAX_MEMORY_SIZE = 1024;

class MemoryAlloc;

//内存块
class MemoryBlock
{
public:
	//内存块编号
	int nID;
	//引用次数
	int nRef;
	//所属内存块（池）
	MemoryAlloc* pAlloc;
	//下一块位置
	MemoryBlock* pNext;
	//是否在内存池中
	bool inPool;
};

//内存池
class MemoryAlloc
{
public:
	MemoryAlloc() :pBuf(nullptr), pHead(nullptr),nSize(64),nBlockSize(1000){ }
	~MemoryAlloc()
	{
		if (pBuf)
			free(pBuf);
	}	
	//初始化
	void initMem();
	//申请内存
	void* allocMem(size_t);
	//释放内存
	void freeMem(void*);
protected:
	//内存池地址
	char* pBuf;
	//内存池头部指针
	MemoryBlock* pHead;
	//内存单元大小
	size_t nSize;
	//内存单元数量
	size_t nBlockSize;
	std::mutex m;
};

template<size_t nsize,size_t nblockSize>
class MemoryAllocator : public MemoryAlloc
{
public:
	MemoryAllocator() {
		const int size = sizeof(void*);
		nSize = (nsize / size) * size + (nsize % size ? size:0 );
		nBlockSize = nblockSize;
		//initMem();
	}
};

//内存管理工具
class MemoryManage
{
public:
	//单例
	static MemoryManage& getInstance();
	//申请内存
	void *allocMem(size_t);
	//释放内存
	void freeMem(void*);
private:
	//将构造方法声明为私有
	MemoryManage(){
		init(0, 64, &mem64);
		init(65, 128, &mem128);
		init(129, 256, &mem256);
		init(257, 512, &mem512);
		init(513, 1024, &mem1024);

	}
	~MemoryManage(){}
	MemoryManage(MemoryManage&) = delete;//删除拷贝构造函数
	MemoryManage& operator=(MemoryManage&) = delete;//删除复制运算符
private:
	//内存池映射数组初始化
	void init(int, int, MemoryAlloc *);
	MemoryAllocator<64, 1000000> mem64;
	MemoryAllocator<128, 1000000> mem128;
	MemoryAllocator<256, 100000> mem256;
	MemoryAllocator<512, 100000> mem512;
	MemoryAllocator<1024, 100000> mem1024;
	MemoryAlloc* szAlloc[MAX_MEMORY_SIZE + 1];
};
#endif