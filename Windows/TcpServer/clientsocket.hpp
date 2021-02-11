#ifndef _CLIENTSOCKET_HPP_
#define _CLIENTSOCKET_HPP_
#include "pre.h"
#include "messageHeader.h"



class ClientSocket
{
public:
	ClientSocket(SOCKET sock) :sockfd(sock) {
		memset(recvBuf, 0, RECV_BUFF_SIZE);
		memset(sendBuf, 0, SEND_BUFF_SIZE);
	}
	~ClientSocket() {}
	SOCKET getSock() { return sockfd; }
	char* msgBuf() { return recvBuf; }
	int getRecvPos() { return lastRecvPos; }
	void setRecvPos(int pos) { lastRecvPos = pos; }
	int getSendPos() { return lastSendPos; }
	void setSendPos(int pos) { lastSendPos = pos; }
	int sendData(DataHeader* dh) {
		int ret = SOCKET_ERROR;
		int sendLen = dh->dataLength;
		const char* data = reinterpret_cast<const char*>(dh);
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
private:
	//���ջ�����
	char recvBuf[RECV_BUFF_SIZE] = {};
	//���ͻ�����
	char sendBuf[SEND_BUFF_SIZE] = {};
	int lastRecvPos = 0;
	int lastSendPos = 0;
	SOCKET sockfd;
};

#endif