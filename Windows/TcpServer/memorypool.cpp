#include "memorypool.h"
#include <assert.h>

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
		std::cout << pRet << "\t" << pRet->nID << "  " << size << std::endl;
		return reinterpret_cast<char *>(pRet) + sizeof(MemoryBlock) ;
	}
}

void MemoryManage::freeMem(void* p)
{
	MemoryBlock* pNode =reinterpret_cast<MemoryBlock*>(static_cast<char*>(p) - sizeof(MemoryBlock));
	if (pNode->inPool) {
		pNode->pAlloc->freeMem(p);
	}
	else {
		if(0 == --pNode->nRef)
		free(pNode);
	}
}

void MemoryManage::init(int nBegin, int nEnd, MemoryAlloc *pMem)
{
	for(int i = nBegin; i <= nEnd;++i){
		szAlloc[i] = pMem;
	}
}

void MemoryAlloc::initMem()
{
	//断言 当指针未初始化报错
	assert(nullptr == pBuf);
	//计算内存池大小
	size_t bufSize = (nSize+sizeof(MemoryBlock)) * nBlockSize;
	//向系统申请池内存
	pBuf = static_cast<char *>(malloc(bufSize));
	//初始化内存池
	pHead = reinterpret_cast<MemoryBlock *>(pBuf);
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
		temp->pNext = reinterpret_cast<MemoryBlock * >(reinterpret_cast<char*>(temp) + nSize+sizeof(MemoryBlock));
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
		pRet = static_cast<MemoryBlock *>(malloc(size+sizeof(MemoryBlock)));
		pRet->inPool = false;
		pRet->nID = -1;
		pRet->nRef = 1;
		pRet->pAlloc = this;
		pRet->pNext = nullptr;
		std::cout << pRet <<"\t" << pRet->nID << "  " <<size<< std::endl;
	}
	else {//从内存池中取出一块内存
		pRet = pHead;
		pHead = pHead->pNext;
		assert(0 == pRet->nRef);
		pRet->nRef = 1;
		//xCout << pRet <<"\t" << pRet->nID << "  " << size << std::endl;
	}
	return reinterpret_cast<char *>(pRet) + sizeof(MemoryBlock);
}

void MemoryAlloc::freeMem(void* p)
{
	MemoryBlock* pBlock = reinterpret_cast<MemoryBlock*>(static_cast<char *>(p) - sizeof(MemoryBlock));
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

