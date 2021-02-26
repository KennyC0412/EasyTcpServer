#pragma once

#ifndef _CLIENT_H_
#define _CLIENT_H_
#include "messageHeader.h"
#include "pre.h"
#include "CELLClient.h"
#include "CELLBuffer.h"
#include "FDset.hpp"

class TcpClient {
public:
	TcpClient():isConnect(false)
	{}
	virtual ~TcpClient() { 
			Close();
	}
	//初始化socket
	int initSocket();
	//连接服务器
	int connServer(const char* , short);
	//关闭socket
	void Close();
	//接收数据
	int readData();
	//发送数据
	int writeData(DataHeaderPtr& );
	//处理网络事务
	bool onRun(int);
	bool isRun();
	virtual void onNetMsg(DataHeader*) = 0;
	CELLClientPtr _client = nullptr;

public:
	unsigned int nRecvMsgID = 1;
	unsigned int nSendMsgID = 1;
	bool _bCheckMsgID = false;
private:
	bool isConnect;
	FDset fdRead;
	FDset fdWrite;
};


#endif