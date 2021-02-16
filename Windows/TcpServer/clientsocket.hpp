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
	ClientSocket(SOCKET sock) :sockfd(sock) {
		memset(recvBuf, 0, RECV_BUFF_SIZE);
		memset(sendBuf, 0, SEND_BUFF_SIZE);
		rstDtHeart();
		rstDtSend();
	}
	~ClientSocket() { closesocket(sockfd); }
	SOCKET getSock() { return sockfd; }
	char* msgBuf() { return recvBuf; }
	int getRecvPos() { return lastRecvPos; }
	void setRecvPos(int pos) { lastRecvPos = pos; }
	int getSendPos() { return lastSendPos; }
	void setSendPos(int pos) { lastSendPos = pos; }
	int sendData(DataHeaderPtr& dh) {
		int ret = SOCKET_ERROR;
		int sendLen = dh->dataLength;
		const char* data = reinterpret_cast<const char*>(dh.get());
		while (true) {
			//�������ݳ�����ǰ������ʣ���С
			if (lastSendPos + sendLen >= SEND_BUFF_SIZE) {
				//����Ŀǰ���Կ�����С
				int canCopy = SEND_BUFF_SIZE - lastSendPos;
				//�������ݵ����ͻ�����
				memcpy(sendBuf+lastSendPos, data, canCopy);
				//��λ��ʣ������λ��
				data += canCopy;
				//ʣ�෢�ͳ���
				sendLen -= canCopy;
				//���� ���ͻ���������
				ret = send(sockfd, sendBuf, SEND_BUFF_SIZE, 0);
				rstDtSend();
				lastSendPos = 0;
				if (SOCKET_ERROR == ret) {
					return ret;
				}
			}
			else {//�������ݲ��㻺������С
				memcpy(sendBuf + lastSendPos, data, sendLen);
				lastSendPos += sendLen;
				break;
			}
		}
		return ret;
	}

	int sendData() {
		int ret = SOCKET_ERROR;
		if (lastSendPos > 0 && SOCKET_ERROR != sockfd) {
			ret = send(sockfd, sendBuf, lastSendPos, 0);
			lastSendPos = 0;
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
	char recvBuf[RECV_BUFF_SIZE] = {};
	//���ͻ�����
	char sendBuf[SEND_BUFF_SIZE] = {};
	int lastRecvPos = 0;
	int lastSendPos = 0;
	SOCKET sockfd;
	time_t dtHeart;
	time_t dtSend;
};

#endif