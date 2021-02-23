#ifndef _MEMORYPOOL_H_
#define _MEMORYPOOL_H_
#include <cstdlib>
#include <mutex>
#include <iostream>
#include <assert.h>

const int MAX_MEMORY_SIZE = 1024;

class MemoryAlloc;

//�ڴ��
class MemoryBlock
{
public:
	//�ڴ����
	int nID;
	//���ô���
	int nRef;
	//�����ڴ�飨�أ�
	MemoryAlloc* pAlloc;
	//��һ��λ��
	MemoryBlock* pNext;
	//�Ƿ����ڴ����
	bool inPool;
};

//�ڴ��
class MemoryAlloc
{
public:
	MemoryAlloc() :pBuf(nullptr), pHead(nullptr),nSize(64),nBlockSize(1000){ }
	~MemoryAlloc()
	{
		if (pBuf)
			free(pBuf);
	}	
	//��ʼ��
	void initMem();
	//�����ڴ�
	void* allocMem(size_t);
	//�ͷ��ڴ�
	void freeMem(void*);
protected:
	//�ڴ�ص�ַ
	char* pBuf;
	//�ڴ��ͷ��ָ��
	MemoryBlock* pHead;
	//�ڴ浥Ԫ��С
	size_t nSize;
	//�ڴ浥Ԫ����
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

//�ڴ������
class MemoryManage
{
public:
	//����
	static MemoryManage& getInstance();
	//�����ڴ�
	void *allocMem(size_t);
	//�ͷ��ڴ�
	void freeMem(void*);
private:
	//�����췽������Ϊ˽��
	MemoryManage(){
		init(0, 64, &mem64);
		init(65, 128, &mem128);
		init(129, 256, &mem256);
		init(257, 512, &mem512);
		init(513, 1024, &mem1024);

	}
	~MemoryManage(){}
	MemoryManage(MemoryManage&) = delete;//ɾ���������캯��
	MemoryManage& operator=(MemoryManage&) = delete;//ɾ�����������
private:
	//�ڴ��ӳ�������ʼ��
	void init(int, int, MemoryAlloc *);
	MemoryAllocator<64, 1000000> mem64;
	MemoryAllocator<128, 1000000> mem128;
	MemoryAllocator<256, 100000> mem256;
	MemoryAllocator<512, 100000> mem512;
	MemoryAllocator<1024, 100000> mem1024;
	MemoryAlloc* szAlloc[MAX_MEMORY_SIZE + 1];
};


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
	//���� ��ָ��δ��ʼ������
	assert(nullptr == pBuf);
	//�����ڴ�ش�С
	size_t bufSize = (nSize + sizeof(MemoryBlock)) * nBlockSize;
	//��ϵͳ������ڴ�
	pBuf = static_cast<char*>(malloc(bufSize));
	//��ʼ���ڴ��
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
	if (nullptr == pHead) {//�ڴ���þ�����ϵͳ�з���һ���ڴ�
		pRet = static_cast<MemoryBlock*>(malloc(size + sizeof(MemoryBlock)));
		pRet->inPool = false;
		pRet->nID = -1;
		pRet->nRef = 1;
		pRet->pAlloc = this;
		pRet->pNext = nullptr;
		//std::cout << pRet <<"\t" << pRet->nID << "  " <<size<< std::endl;
	}
	else {//���ڴ����ȡ��һ���ڴ�
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
		//ͷ������������߳��޸� �����̰߳�ȫ������ʽϴ�
		std::lock_guard<std::mutex> lk(m);
		pBlock->pNext = pHead;
		pHead = pBlock;
	}
	else {
		free(pBlock);
	}
}


#endif