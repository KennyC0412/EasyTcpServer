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
	//数据写入缓冲区等待可发送
	int writeData(DataHeaderPtr& dh) {
		int ret = SOCKET_ERROR;
		int sendLen = dh->dataLength;
		const char* data = reinterpret_cast<const char*>(dh.get());
		if (_lastSendPos + sendLen <= RECV_BUFF_SIZE) {
			//拷贝接收数据到发送缓冲区
			memcpy(_sendBuf + _lastSendPos, data , sendLen);
			//定位到缓冲区尾部
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
	//发送缓冲区数据
	int sendData() {
		int ret = 0;
		//缓冲区有数据
		if (_lastSendPos > 0 && INVALID_SOCKET != _sockfd) {
			ret = send(_sockfd, _sendBuf, _lastSendPos, 0);
			//重置缓冲区写满次数
			_sendBufFullCount = 0;
			//重置数据尾部位置
			_lastSendPos = 0;
			//重置定时发送时间
			rstDtSend();
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
	char _recvBuf[RECV_BUFF_SIZE] = {};
	//发送缓冲区
	char _sendBuf[SEND_BUFF_SIZE] = {};
	int _lastRecvPos = 0;
	int _lastSendPos = 0;
	//发送缓冲区已满次数
	int _sendBufFullCount = 0;
	SOCKET _sockfd;
	time_t dtHeart;
	time_t dtSend;

};

#endif