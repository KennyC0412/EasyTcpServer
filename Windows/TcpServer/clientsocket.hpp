#ifndef _CLIENTSOCKET_HPP_
#define _CLIENTSOCKET_HPP_
#include "pre.h"
#include "messageHeader.h"
#include "ObjectPool.hpp"

//心跳检测时间
#define CLIENT_HEART_DEAD_TIME 60000
//缓冲区内缓存消息发送给客户端的时间
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
				rstDtSend();
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
	//心跳检测检查
	bool checkHeart(time_t t) {
			dtHeart += t;
			return dtHeart >= CLIENT_HEART_DEAD_TIME?true : false;
	}
	//定时发送检查
	void checkSend(time_t t) {
		dtSend += t;
		if (dtSend >= CLIENT_SEND_BUFF_TIME) {
			//立即发送数据
			sendData();
			rstDtSend();
		}
	}

private:
	//接收缓冲区
	char recvBuf[RECV_BUFF_SIZE] = {};
	//发送缓冲区
	char sendBuf[SEND_BUFF_SIZE] = {};
	int lastRecvPos = 0;
	int lastSendPos = 0;
	SOCKET sockfd;
	time_t dtHeart;
	time_t dtSend;
};

#endif