#ifdef _WIN32
	#pragma once
#endif
#ifndef _SERVER_H_
#define _SERVER_H_
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <Windows.h>
	#include <WinSock2.h>
#else
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <string.h>
	#define SOCKET int
	#define INVALID_SOCKET (SOCKET)(~0)
	#define SOCKET_ERROR				     (-1)
#endif

#include <iostream>
#include <vector>
#include "messageHeader.h"
#include "pre.h"
#include <algorithm>
class TcpServer {
public:
	TcpServer():s_sock(INVALID_SOCKET){}
	virtual ~TcpServer() { closeServer(); }
	//创建套接字
	int initSocket();
	//绑定套接字
	int bindSocket(const char* ip = nullptr, unsigned short = PORT);
	//监听端口
	int listenPort(int backlog = BACKLOG);
	//接受连接
	int acConnection();
	//接收数据
	int recvData(SOCKET);
	//发送数据
	int sendData(SOCKET,DataHeader *);
	//群发消息
	void sendToAll(DataHeader *);
	//处理事务
	bool onRun();
	//关闭套接字
	void closeSocket(SOCKET);
	//响应消息
	virtual void resData(DataHeader *,SOCKET );
	//判断
	bool isRun();
	//
	void closeServer();
private:
	SOCKET s_sock;
	std::vector<SOCKET> g_clients;
};

#endif // !_SERVER_H_
