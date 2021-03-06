#pragma once

#ifndef _CLIENT_H_
#define _CLIENT_H_
#define WIN32_LEAN_AND_MEAN
#ifdef _WIN32
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
#include <thread>
#include <vector>
#include "messageHeader.h"

class TcpClient {
public:
	TcpClient():c_sock(INVALID_SOCKET) {}
	virtual ~TcpClient() { 
		if(c_sock != INVALID_SOCKET)
			closeSocket();
	}
	//初始化socket
	int initSocket();
	//连接服务器
	int connServer(const char* , short );
	//关闭socket
	void closeSocket();
	//接收数据
	int recvData(SOCKET);
	//发送数据
	int sendData(DataHeader *);
	//响应数据
	void resData(DataHeader *);
	//状态查询
	bool OnRun();
	//
	bool isRun();
	SOCKET get_sock() { return c_sock; }
private:
	SOCKET c_sock;
	sockaddr_in _sin;
	std::vector<int> g_clients;
};

#endif