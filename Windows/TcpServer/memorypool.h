#pragma once
#ifndef _MEMORYPOOL_H_
#define _MEMORYPOOL_H_
#include <cstdlib>
#include <mutex>
#include <iostream>

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
#endif