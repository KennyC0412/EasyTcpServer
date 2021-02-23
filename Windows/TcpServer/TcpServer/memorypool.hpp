#ifndef _MEMORYPOOL_H_
#define _MEMORYPOOL_H_
#include <cstdlib>
#include <mutex>
#include <iostream>
#include <assert.h>

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


MemoryManage& MemoryManage::getInstance()
{//单例模式 内部静态成员 一直存在
	static MemoryManage m;
	return m;
}

void* MemoryManage::allocMem(size_t size)
{
	if (size <= MAX_MEMORY_SIZE) {
		//使用内存池中的内存进行分配
		return szAlloc[size]->allocMem(size);
	}
	else {//从系统中分配一块内存
		MemoryBlock* pRet = static_cast<MemoryBlock*>(malloc(size + sizeof(MemoryBlock)));
		pRet->inPool = false;
		pRet->nID = -1;
		pRet->nRef = 1;
		pRet->pAlloc = nullptr;
		pRet->pNext = nullptr;
		//std::cout << pRet << "\t" << pRet->nID << "  " << size << std::endl;
		return reinterpret_cast<char*>(pRet) + sizeof(MemoryBlock);
	}
}

void MemoryManage::freeMem(void* p)
{
	MemoryBlock* pNode = reinterpret_cast<MemoryBlock*>(static_cast<char*>(p) - sizeof(MemoryBlock));
	if (pNode->inPool) {
		pNode->pAlloc->freeMem(p);
	}
	else {
		if (0 == --pNode->nRef)
			free(pNode);
	}
}

void MemoryManage::init(int nBegin, int nEnd, MemoryAlloc* pMem)
{
	for (int i = nBegin; i <= nEnd; ++i) {
		szAlloc[i] = pMem;
	}
}

void MemoryAlloc::initMem()
{
	//断言 当指针未初始化报错
	assert(nullptr == pBuf);
	//计算内存池大小
	size_t bufSize = (nSize + sizeof(MemoryBlock)) * nBlockSize;
	//向系统申请池内存
	pBuf = static_cast<char*>(malloc(bufSize));
	//初始化内存池
	pHead = reinterpret_cast<MemoryBlock*>(pBuf);
	if (pHead == nullptr)
		return;
	else {
		pHead->inPool = true;
		pHead->nID = 0;
		pHead->nRef = 0;
		pHead->pAlloc = this;
		pHead->pNext = nullptr;
	}

	MemoryBlock* temp = pHead;
	for (size_t i = 1; i < nBlockSize; ++i) {
		temp->pNext = reinterpret_cast<MemoryBlock*>(reinterpret_cast<char*>(temp) + nSize + sizeof(MemoryBlock));
		temp = temp->pNext;
		temp->inPool = true;
		temp->nID = i;
		temp->nRef = 0;
		temp->pAlloc = this;
	}
	temp->pNext = nullptr;
}

void* MemoryAlloc::allocMem(size_t size)
{
	m.lock();
	if (!pBuf) {
		initMem();
	}
	m.unlock();
	MemoryBlock* pRet = nullptr;
	std::lock_guard<std::mutex> lk(m);
	if (nullptr == pHead) {//内存池用尽，从系统中分配一块内存
		pRet = static_cast<MemoryBlock*>(malloc(size + sizeof(MemoryBlock)));
		pRet->inPool = false;
		pRet->nID = -1;
		pRet->nRef = 1;
		pRet->pAlloc = this;
		pRet->pNext = nullptr;
		//std::cout << pRet <<"\t" << pRet->nID << "  " <<size<< std::endl;
	}
	else {//从内存池中取出一块内存
		pRet = pHead;
		pHead = pHead->pNext;
		assert(0 == pRet->nRef);
		pRet->nRef = 1;
		//xCout << pRet <<"\t" << pRet->nID << "  " << size << std::endl;
	}
	return reinterpret_cast<char*>(pRet) + sizeof(MemoryBlock);
}

void MemoryAlloc::freeMem(void* p)
{
	MemoryBlock* pBlock = reinterpret_cast<MemoryBlock*>(static_cast<char*>(p) - sizeof(MemoryBlock));
	assert(1 == pBlock->nRef);
	if (0 != --pBlock->nRef) {
		return;
	}
	if (pBlock->inPool) {
		//头部经常被多个线程修改 出现线程安全问题概率较大
		std::lock_guard<std::mutex> lk(m);
		pBlock->pNext = pHead;
		pHead = pBlock;
	}
	else {
		free(pBlock);
	}
}


#endif