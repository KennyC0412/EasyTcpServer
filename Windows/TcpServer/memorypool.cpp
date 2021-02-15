#include "memorypool.h"
#include <assert.h>

MemoryManage& MemoryManage::getInstance()
{//����ģʽ �ڲ���̬��Ա һֱ����
	static MemoryManage m;
	return m;
}

void* MemoryManage::allocMem(size_t size)
{
	if (size <= MAX_MEMORY_SIZE) {
		//ʹ���ڴ���е��ڴ���з���
		return szAlloc[size]->allocMem(size);
	}
	else {//��ϵͳ�з���һ���ڴ�
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
	//���� ��ָ��δ��ʼ������
	assert(nullptr == pBuf);
	//�����ڴ�ش�С
	size_t bufSize = (nSize+sizeof(MemoryBlock)) * nBlockSize;
	//��ϵͳ������ڴ�
	pBuf = static_cast<char *>(malloc(bufSize));
	//��ʼ���ڴ��
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
	if (nullptr == pHead) {//�ڴ���þ�����ϵͳ�з���һ���ڴ�
		pRet = static_cast<MemoryBlock *>(malloc(size+sizeof(MemoryBlock)));
		pRet->inPool = false;
		pRet->nID = -1;
		pRet->nRef = 1;
		pRet->pAlloc = this;
		pRet->pNext = nullptr;
		std::cout << pRet <<"\t" << pRet->nID << "  " <<size<< std::endl;
	}
	else {//���ڴ����ȡ��һ���ڴ�
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
		//ͷ������������߳��޸� �����̰߳�ȫ������ʽϴ�
		std::lock_guard<std::mutex> lk(m);	
		pBlock->pNext = pHead;
		pHead = pBlock;
	}
	else {
		free(pBlock);
	}
}

