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
			//发送数据超出当前缓冲区剩余大小
			if (lastSendPos + sendLen >= SEND_BUFF_SIZE) {
				//计算目前可以拷贝大小
				int canCopy = SEND_BUFF_SIZE - lastSendPos;
				//拷贝数据到发送缓冲区
				memcpy(sendBuf+lastSendPos, data, canCopy);
				//定位到剩余数据位置
				data += canCopy;
				//剩余发送长度
				sendLen -= canCopy;
				//发送 发送缓冲区数据
				ret = send(sockfd, sendBuf, SEND_BUFF_SIZE, 0);
				lastSendPos = 0;
				if (SOCKET_ERROR == ret) {
					return ret;
				}
			}
			else {//发送数据不足缓冲区大小
				memcpy(sendBuf + lastSendPos, data, sendLen);
				lastSendPos += sendLen;
				break;
			}
		}
		return ret;
	}
private:
	//接收缓冲区
	char recvBuf[RECV_BUFF_SIZE] = {};
	//发送缓冲区
	char sendBuf[SEND_BUFF_SIZE] = {};
	int lastRecvPos = 0;
	int lastSendPos = 0;
	SOCKET sockfd;
};

#endif