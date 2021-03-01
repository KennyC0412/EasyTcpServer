#ifndef _CELL_BUFFER_H_
#define _CELL_BUFFER_H_
#include "preDefine.h"
#include "MsgHeader.h"

class CELLBuffer 
{
public:
	CELLBuffer(int size): _nSize(size)
	{
		_BufPtr = new char[_nSize];
	}
	~CELLBuffer() {
		if (_BufPtr) {
			_BufPtr = nullptr;
		}
	}
	char * data() { return _BufPtr; }
	//�򻺳���д������
	bool push(const char*,int);
	int sendData(SOCKET);
	int recvData(SOCKET);
	bool hasMsg();
	void pop(int);
	inline bool needWrite() { return _nSize - _lastPos > 0; }
private:
	//���ͻ�����
	char *_BufPtr = nullptr;
	//������β��λ��
	int _lastPos = 0;
	//��������С
	int _nSize = RECV_BUFF_SIZE;
	//���ͻ�������������
	int _bufFullCount = 0;
};
#endif // !_CELL_BUFFER_H_
