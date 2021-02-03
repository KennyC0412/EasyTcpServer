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
	//��ʼ��socket
	int initSocket();
	//���ӷ�����
	int connServer(const char* , short );
	//�ر�socket
	void closeSocket();
	//��������
	int recvData(SOCKET);
	//��������
	int sendData(DataHeader *);
	//��Ӧ����
	void resData(DataHeader *);
	//״̬��ѯ
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