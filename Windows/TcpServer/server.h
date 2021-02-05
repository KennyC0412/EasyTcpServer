#ifdef _WIN32
	#pragma once
#endif
#ifndef _SERVER_H_
#define _SERVER_H_
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define FD_SETSIZE 1024
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

#include "messageHeader.h"
#include "pre.h"
#include "CELLtimestamp.hpp"
#include <algorithm>


class ClientSocket
{
public:
	ClientSocket(SOCKET sock):sockfd(sock){}
	~ClientSocket(){}
	SOCKET getSock() { return sockfd; }
	char* msgBuf() { return szMsgBuf; }
	int getPos() { return lastPos; }
	void setPos(int pos) { lastPos = pos; }
private:
	SOCKET sockfd;
	char szMsgBuf[RECV_BUFF_SIZE * 10] = {};
	int lastPos = 0;
};


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
	int recvData(ClientSocket* client);
	//发送数据
	int sendData(SOCKET,DataHeader *);
	//群发消息
	void sendToAll(DataHeader *);
	//处理事务
	bool onRun();
	//关闭套接字
	void closeSocket(SOCKET);
	//响应消息
	virtual void onNetMsg(DataHeader *,SOCKET );
	//判断
	bool isRun();
	//
	void closeServer();
private:
	SOCKET s_sock;
	std::vector<ClientSocket *> g_clients;
	//接收缓冲区
	char szRecv[RECV_BUFF_SIZE] = {};
	CELLTimestamp tTime;
};

#endif // !_SERVER_H_
