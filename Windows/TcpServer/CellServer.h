#pragma once

#ifndef _CELLSERVER_H_
#define _CELLSERVER_H_

#include "pre.h"
#include "CELLTask.h"
#include "clientsocket.hpp"
#include "messageHeader.h"

class CellServer;
class INetEvent;
using sendMsg2ClientPtr = std::shared_ptr<sendMsg2Client>;
using CellServerPtr = std::shared_ptr<CellServer>;
using LoginResultPtr = std::shared_ptr<LoginResult>;
using DataHeaderPtr = std::shared_ptr<DataHeader>;
class CellServer
{
public:
	CellServer(SOCKET sock = INVALID_SOCKET) :s_sock(sock), /*pThread(nullptr)*/ pINetEvent(nullptr) {}
	~CellServer() { std::cout << "CellServer Closed."<<std::endl; closeServer(); }
	//����������Ϣ
	void onRun();
	//��������
	int recvData(ClientSocketPtr& client);
	//��Ӧ��Ϣ
	void onNetMsg(ClientSocketPtr& pclient, DataHeader* dh);
	//�رշ�����
	void closeServer();
	//��ӿͻ���
	void addClient(ClientSocketPtr client);
	void setEventObj(INetEvent* event) { pINetEvent = event; }
	void Start();
	void sendTask(ClientSocketPtr& ,DataHeaderPtr &);
	size_t getClientCount() { return g_clients.size() + clientsBuffer.size(); }
	void readData(fd_set&);
	void CheckTime();
private:
	CELLTimestamp tTime;
	CellTaskServer taskServer;
	//std::thread* pThread;
	INetEvent* pINetEvent;
	SOCKET s_sock;
	//��ʽ�ͻ�����
	std::map<SOCKET, ClientSocketPtr> g_clients;
	//����ͻ�����
	std::vector<ClientSocketPtr> clientsBuffer{};
	std::mutex m;
	Semaphore sem;
	bool client_change = true;
	bool isRun = false;
};

#endif