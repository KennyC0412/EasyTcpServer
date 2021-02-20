#ifdef _WIN32
#pragma once
#endif

#ifndef _CELL_CLIENT_H_
#define _CELL_CLIENT_H_

#include "pre.h"
#include "messageHeader.h"
#include "ObjectPool.hpp"
#include "CELLBuffer.h"

//心跳检测时间
#define CLIENT_HEART_DEAD_TIME 60000
//缓冲区内缓存消息发送给客户端的时间
#define CLIENT_SEND_BUFF_TIME 200


class CELLClient:public ObjectPoolBase<CELLClient,10000>
{
public:
	CELLClient(SOCKET sock) :_sockfd(sock){
		
		rstDtHeart();
		rstDtSend();
	}
	~CELLClient() { closesocket(_sockfd); }
	SOCKET getSock() { return _sockfd; }
	//数据写入缓冲区等待可发送
	int push(DataHeaderPtr&);
	//移除头部消息
	void pop_front_Msg();
	//发送缓冲区数据
	int sendData();
	//接受数据
	int recvData();
	//重置心跳时间
	void rstDtHeart() { dtHeart = 0; }
	//重置发送时间
	void rstDtSend() { dtSend = 0; }
	//心跳检测检查
	bool checkHeart(time_t);
	//定时发送检查
	void checkSend(time_t);
	//检查是否有消息
	bool hasMsg();
	//得到缓冲区头部数据
	DataHeader* front_Msg();
private:
	//发送缓冲区
	CELLBuffer _sendBuff = CELLBuffer(SEND_BUFF_SIZE);
	//接收缓冲区
	CELLBuffer _recvBuff = CELLBuffer(RECV_BUFF_SIZE);
	SOCKET _sockfd;
	time_t dtHeart;
	time_t dtSend;

};
using CELLClientPtr = std::shared_ptr<CELLClient>;

#endif // !_CELL_CLIENT_H_
