#ifdef _WIN32
#pragma once
#endif

#ifndef _CELL_BUFFER_H_
#define _CELL_BUFFER_H_
#include "pre.h"
#include "messageHeader.h"

class CELLBuffer 
{
public:
	CELLBuffer(int size): _nSize(size)
	{
		_BufPtr = new char[_nSize];
	}
	~CELLBuffer() { 
		if (_BufPtr) {
			delete _BufPtr;
			_BufPtr = nullptr;
		}
	}
	char * data() { return _BufPtr; }
	//向缓冲区写入数据
	bool push(const char*,int);
	int sendData(SOCKET);
	int recvData(SOCKET);
	bool hasMsg();
	void pop(int);
	inline bool needWrite() { return _nSize - _lastPos > 0; }
private:
	//发送缓冲区
	char *_BufPtr = nullptr;
	//缓冲区尾部位置
	int _lastPos = 0;
	//缓冲区大小
	int _nSize = RECV_BUFF_SIZE;
	//发送缓冲区已满次数
	int _bufFullCount = 0;
};
#endif // !_CELL_BUFFER_H_
