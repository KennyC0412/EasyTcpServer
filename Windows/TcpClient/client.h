#pragma once

#ifndef _CLIENT_H_
#define _CLIENT_H_
#include "messageHeader.h"
#include "pre.h"
#include "CELLClient.h"
#include "CELLBuffer.h"

class TcpClient {
public:
	TcpClient():isConnect(false)
	{
	}
	virtual ~TcpClient() { 
			Close();
	}
	//初始化socket
	int initSocket();
	//连接服务器
	int connServer(const char* , short = PORT );
	//关闭socket
	void Close();
	//接收数据
	int readData();
	//发送数据
	int writeData(DataHeaderPtr& );
	//处理网络事务
	void onRun();
	bool isRun();
	virtual void onNetMsg(DataHeader*) = 0;
protected:
	CELLClientPtr _client = nullptr;
	bool isConnect;
};


#endif