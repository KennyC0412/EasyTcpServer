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
	//�����׽���
	int initSocket();
	//���׽���
	int bindSocket(const char* ip = nullptr, unsigned short = PORT);
	//�����˿�
	int listenPort(int backlog = BACKLOG);
	//��������
	int acConnection();
	//��������
	int recvData(SOCKET);
	//��������
	int sendData(SOCKET,DataHeader *);
	//Ⱥ����Ϣ
	void sendToAll(DataHeader *);
	//��������
	bool onRun();
	//�ر��׽���
	void closeSocket(SOCKET);
	//��Ӧ��Ϣ
	virtual void resData(DataHeader *,SOCKET );
	//�ж�
	bool isRun();
	//
	void closeServer();
private:
	SOCKET s_sock;
	std::vector<SOCKET> g_clients;
};

#endif // !_SERVER_H_
