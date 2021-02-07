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
#include <atomic>

class ClientSocket
{
public:
	ClientSocket(SOCKET sock):sockfd(sock){}
	~ClientSocket(){}
	SOCKET getSock() { return sockfd; }
	char* msgBuf() { return szMsgBuf; }
	int getPos() { return lastPos; }
	void setPos(int pos) { lastPos = pos; }
	SOCKET sockfd;
private:
	
	char szMsgBuf[RECV_BUFF_SIZE * 10] = {};
	int lastPos = 0;
};

class CellServer {
public:
	CellServer(SOCKET sock = INVALID_SOCKET) :s_sock(sock), pThread(nullptr) {}
	~CellServer() { closeServer(); }
	//处理网络消息
	bool onRun();
	bool isRun() { return s_sock != INVALID_SOCKET; }
	//接收数据
	int recvData(ClientSocket* client);
	//响应消息
	virtual void onNetMsg(DataHeader*, SOCKET);
	void closeServer();
	void addClient(ClientSocket* client);
	void Start();
	size_t getClientCount() { return g_clients.size() + clientsBuffer.size(); }
private:
	SOCKET s_sock;
	//接收缓冲区
	char szRecv[RECV_BUFF_SIZE] = {};
	//正式客户队列
	std::vector<ClientSocket*> g_clients;
	//缓冲客户队列
	std::vector<ClientSocket*> clientsBuffer;
	std::mutex m;
	std::thread* pThread;
	CELLTimestamp tTime;
public :
	std::atomic_int recvCount = 0;
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
	//发送数据
	int sendData(SOCKET,DataHeader *);
	//群发消息
	void sendToAll(DataHeader *);
	//关闭套接字
	void closeSocket(SOCKET);
	void Start();
	//判断
	bool isRun();
	//响应消息
	void time4msg();
	bool onRun();
	void closeServer();
	void addClientToServer(ClientSocket* client);
private:
	SOCKET s_sock;
	std::vector<ClientSocket *> g_clients;
	std::vector<CellServer *> g_servers;
	CELLTimestamp tTime;
	std::atomic_int recvCount = 0;
};

#endif // !_SERVER_H_
