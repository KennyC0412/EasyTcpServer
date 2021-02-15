#pragma once
#ifndef  _OBJECT_POOL_H_
#define _OBJECT_POOL_H_

#include <mutex>

template<class T,size_t nPoolSize>
class ObjectPool 
{
private:
	class NodeHeader
	{
	public:
		//�ڴ����
		int nID;
		//���ô���
		char nRef;
		//��һ��λ��
		NodeHeader* pNext;
		//�Ƿ����ڴ����
		bool inPool;
	};
	//��ʼ��
	void initPool() {
		if (pBuf) {
			return;
		}
		size_t size = nPoolSize * (sizeof(NodeHeader) + sizeof(T));
		pBuf = new char[size];
		pHeader = reinterpret_cast<NodeHeader*>(pBuf);
		pHeader->nID = 0;
		pHeader->nRef = 0;
		pHeader->inPool = true;
		NodeHeader* temp = pHeader;
		for (size_t i = 1; i < nPoolSize; ++i) {
			temp->pNext = reinterpret_cast<NodeHeader*>(reinterpret_cast<char*>(temp) + sizeof(NodeHeader) + sizeof(T));
			temp = temp->pNext;
			temp->nID = i;
			temp->nRef = 0;
			temp->inPool = true;
		}
		temp->pNext = nullptr;
	}
public:
	ObjectPool() :pHeader(nullptr), pBuf(nullptr) { initPool(); }
	~ObjectPool(){delete pBuf;}
	//�������
	void * allocObj() 
	{
		NodeHeader* pRet = nullptr;
		if (nullptr == pHeader) 
		{
			pRet = reinterpret_cast<NodeHeader*>(new char[sizeof(T) + sizeof(NodeHeader)]);
			pRet->inPool = false;
			pRet->nID = -1;
			pRet->nRef = 1;
			pRet->pNext = nullptr;
		}
		else {
			pRet = pHeader;
			pHeader = pHeader->pNext;
			pRet->nRef = 1;
			//std::cout << pRet << "\t ID:" << pRet->nID<< std::endl;
		}
		return reinterpret_cast<char*>(pRet) + sizeof(NodeHeader);
	}
	//�ͷŶ���
	void freeObj(void *obj)
	{
		NodeHeader* pObj =reinterpret_cast<NodeHeader *>(reinterpret_cast<char*>(obj) - sizeof(NodeHeader));
		if (0 != --pObj->nRef) {
			return;
		}
		if (pObj->inPool) {
			std::lock_guard<std::mutex> lk(m);
			pObj->pNext = pHeader;
			pHeader = pObj;
			//std::cout<<"free:" << pObj << "\t ID:" << pObj->nID << std::endl;
		}
		else {
			delete [] pObj;
		}
	}
private:
	//�����ͷָ��
	NodeHeader* pHeader;
	//������ڴ��ַ
	char* pBuf;
	std::mutex m;
};


template<class T, size_t nPoolSize>
class ObjectPoolBase
{
public:
	void* operator new(size_t size)
	{
		return getInstance().allocObj();
	}
	void operator delete(void* obj)
	{
		getInstance().freeObj(obj);
	}
	template< typename ...Args>
	static T* createObject(Args...args)
	{
		T* obj = new T(args...);
		return obj;
	}

	static void destroyObject(T* obj)
	{
		delete obj;
	}

private:
	static ObjectPool<T, nPoolSize>& getInstance()
	{
		static ObjectPool<T, nPoolSize> pool;
		return pool;
	}
};

#endif // ! _OBJECT_POOL_H_

