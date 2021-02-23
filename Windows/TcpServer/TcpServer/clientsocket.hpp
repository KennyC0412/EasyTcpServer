#ifndef _CLIENTSOCKET_HPP_
#define _CLIENTSOCKET_HPP_
#include "pre.h"
#include "messageHeader.h"
#include "ObjectPool.hpp"

//�������ʱ��
#define CLIENT_HEART_DEAD_TIME 60000
//�������ڻ�����Ϣ���͸��ͻ��˵�ʱ��
#define CLIENT_SEND_BUFF_TIME 200

using DataHeaderPtr = std::shared_ptr<DataHeader>;
class ClientSocket:public ObjectPoolBase<ClientSocket,10000>
{
public:
	ClientSocket(SOCKET sock) :_sockfd(sock) {
		memset(_recvBuf, 0, RECV_BUFF_SIZE);
		memset(_sendBuf, 0, SEND_BUFF_SIZE);
		rstDtHeart();
		rstDtSend();
	}
	~ClientSocket() { closesocket(_sockfd); }
	SOCKET getSock() { return _sockfd; }
	char* msgBuf() { return _recvBuf; }
	int getRecvPos() { return _lastRecvPos; }
	void setRecvPos(int pos) { _lastRecvPos = pos; }
	int getSendPos() { return _lastSendPos; }
	void setSendPos(int pos) { _lastSendPos = pos; }
	//����д�뻺�����ȴ��ɷ���
	int writeData(DataHeaderPtr& dh) {
		int ret = SOCKET_ERROR;
		int sendLen = dh->dataLength;
		const char* data = reinterpret_cast<const char*>(dh.get());
		if (_lastSendPos + sendLen <= RECV_BUFF_SIZE) {
			//�����������ݵ����ͻ�����
			memcpy(_sendBuf + _lastSendPos, data , sendLen);
			//��λ��������β��
			_lastSendPos += sendLen;
			if (_lastSendPos == SEND_BUFF_SIZE) {
				_sendBufFullCount++;
			}
			return sendLen;
		}
		else {
			_sendBufFullCount++;
		}
		return ret;
	}
	//���ͻ���������
	int sendData() {
		int ret = 0;
		//������������
		if (_lastSendPos > 0 && INVALID_SOCKET != _sockfd) {
			ret = send(_sockfd, _sendBuf, _lastSendPos, 0);
			//���û�����д������
			_sendBufFullCount = 0;
			//��������β��λ��
			_lastSendPos = 0;
			//���ö�ʱ����ʱ��
			rstDtSend();
		}
		return ret;
	}

	void rstDtHeart() { dtHeart = 0; }
	void rstDtSend() { dtSend = 0; }
	//���������
	bool checkHeart(time_t t) {
			dtHeart += t;
			return dtHeart >= CLIENT_HEART_DEAD_TIME?true : false;
	}
	//��ʱ���ͼ��
	void checkSend(time_t t) {
		dtSend += t;
		if (dtSend >= CLIENT_SEND_BUFF_TIME) {
			//������������
			sendData();
			rstDtSend();
		}
	}

private:
	//���ջ�����
	char _recvBuf[RECV_BUFF_SIZE] = {};
	//���ͻ�����
	char _sendBuf[SEND_BUFF_SIZE] = {};
	int _lastRecvPos = 0;
	int _lastSendPos = 0;
	//���ͻ�������������
	int _sendBufFullCount = 0;
	SOCKET _sockfd;
	time_t dtHeart;
	time_t dtSend;

};

#endif