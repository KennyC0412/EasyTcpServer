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
	//�����׽���
	int initSocket();
	//���׽���
	int bindSocket(const char* ip = nullptr, unsigned short = PORT);
	//�����˿�
	int listenPort(int backlog = BACKLOG);
	//��������
	int acConnection();
	//��������
	int recvData(ClientSocket* client);
	//��������
	int sendData(SOCKET,DataHeader *);
	//Ⱥ����Ϣ
	void sendToAll(DataHeader *);
	//��������
	bool onRun();
	//�ر��׽���
	void closeSocket(SOCKET);
	//��Ӧ��Ϣ
	virtual void onNetMsg(DataHeader *,SOCKET );
	//�ж�
	bool isRun();
	//
	void closeServer();
private:
	SOCKET s_sock;
	std::vector<ClientSocket *> g_clients;
	//���ջ�����
	char szRecv[RECV_BUFF_SIZE] = {};
	CELLTimestamp tTime;
};

#endif // !_SERVER_H_
