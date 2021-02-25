#include "CELLBuffer.h"

bool CELLBuffer::push(const char*pData, int sendLen)
{
	////根据业务需求拓展
	//if (_lastPos + sendLen > _nSize) {
	//	//需要写入数据大于缓冲区可用空间
	//	int n = (_lastPos + sendLen) - _nSize;
	//	if (n < 8192)
	//		n = 8192;
	//	char* buff = new char[_nSize + n];
	//	memcpy(buff, _BufPtr, _nSize);
	//	delete[]_BufPtr;
	//	_BufPtr = buff;
	//}
	if (_lastPos + sendLen <= _nSize) {
		//拷贝接收数据到发送缓冲区
		memcpy(_BufPtr + _lastPos, pData, sendLen);
		//定位到缓冲区尾部
		_lastPos += sendLen;
		if (_lastPos == _nSize) {
			_bufFullCount++;
		}
		return true;
	}
	else {
		_bufFullCount++;
		return false;
	}
}

int CELLBuffer::sendData(SOCKET sockfd)
{
	int ret = 0;
	if (_lastPos > 0 && INVALID_SOCKET != sockfd) {
		ret = send(sockfd, _BufPtr, _lastPos, 0);
		_lastPos = 0;
		_bufFullCount = 0;
	}
	return ret;
}

int CELLBuffer::recvData(SOCKET sockfd)
{
	if (_nSize - _lastPos > 0) {
		//接受客户端数据
		char* szRecv = _BufPtr + _lastPos;
		int recvLen = recv(sockfd, szRecv, _nSize - _lastPos, 0);
		if (recvLen <= 0) {
			return recvLen;
		}
		//缓冲区位置后移
		_lastPos += recvLen;
		return recvLen;
	}
	return 0;
}

bool CELLBuffer::hasMsg()
{
	if (_lastPos >= sizeof(DataHeader)) {
		DataHeader* header = reinterpret_cast<DataHeader*>(_BufPtr);
		return _lastPos >= header->dataLength;
	}
	return false;
}

void CELLBuffer::pop(int nLen)
{
	int n = _lastPos - nLen;
	if (n > 0) {
		memcpy(_BufPtr, _BufPtr+nLen, n);
	}
	_lastPos = n;
	if (_bufFullCount > 0) {
		--_bufFullCount;
	}
}

