#include "CELLBuffer.h"

bool CELLBuffer::push(const char* pData, int sendLen)
{
	if (_lastPos + sendLen <= _nSize) {
		//�����������ݵ����ͻ�����
		memcpy(_BufPtr + _lastPos, pData, sendLen);
		//��λ��������β��
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
		if(ret <= 0){
		return SOCKET_ERROR;
		}
	else if (ret == _lastPos) {
		_lastPos = 0;
	}
	else {
		_lastPos -= ret;
		memcpy(_BufPtr, _BufPtr + ret, _lastPos);
	}
		_bufFullCount = 0;
	}
	return ret;
}

int CELLBuffer::recvData(SOCKET sockfd)
{
	if (_nSize - _lastPos > 0) {
		//���ܿͻ�������
		char* szRecv = _BufPtr + _lastPos;
		int recvLen = recv(sockfd, szRecv, _nSize - _lastPos, 0);
		if (recvLen <= 0) {
			return recvLen;
		}
		//������λ�ú���
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

