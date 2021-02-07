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
	//����������Ϣ
	bool onRun();
	bool isRun() { return s_sock != INVALID_SOCKET; }
	//��������
	int recvData(ClientSocket* client);
	//��Ӧ��Ϣ
	virtual void onNetMsg(DataHeader*, SOCKET);
	void closeServer();
	void addClient(ClientSocket* client);
	void Start();
	size_t getClientCount() { return g_clients.size() + clientsBuffer.size(); }
private:
	SOCKET s_sock;
	//���ջ�����
	char szRecv[RECV_BUFF_SIZE] = {};
	//��ʽ�ͻ�����
	std::vector<ClientSocket*> g_clients;
	//����ͻ�����
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
	//�����׽���
	int initSocket();
	//���׽���
	int bindSocket(const char* ip = nullptr, unsigned short = PORT);
	//�����˿�
	int listenPort(int backlog = BACKLOG);
	//��������
	int acConnection();
	//��������
	int sendData(SOCKET,DataHeader *);
	//Ⱥ����Ϣ
	void sendToAll(DataHeader *);
	//�ر��׽���
	void closeSocket(SOCKET);
	void Start();
	//�ж�
	bool isRun();
	//��Ӧ��Ϣ
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
