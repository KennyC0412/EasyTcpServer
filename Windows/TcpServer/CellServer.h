#pragma once

#ifndef _CELLSERVER_H_
#define _CELLSERVER_H_

#include "pre.h"
#include "server.h"
#include "CELLTask.h"
#include "clientsocket.hpp"

class CellServer
{
public:
	CellServer(SOCKET sock = INVALID_SOCKET) :s_sock(sock), pThread(nullptr), pINetEvent(nullptr) {}
	~CellServer() { closeServer(); }
	//����������Ϣ
	void onRun();
	bool isRun() { return s_sock != INVALID_SOCKET; }
	//��������
	int recvData(ClientSocket* client);
	//��Ӧ��Ϣ
	void onNetMsg(ClientSocket* pclient, DataHeader* dh);
	//�رշ�����
	void closeServer();
	//��ӿͻ���
	void addClient(ClientSocket* client);
	void setEventObj(INetEvent* event) { pINetEvent = event; }
	void Start();
	void sendTask(ClientSocket *,DataHeader *);
	size_t getClientCount() { return g_clients.size() + clientsBuffer.size(); }
private:
	SOCKET s_sock;
	//��ʽ�ͻ�����
	std::map<SOCKET, ClientSocket*> g_clients;
	//����ͻ�����
	std::vector<ClientSocket*> clientsBuffer;
	std::mutex m;
	std::thread* pThread;
	CELLTimestamp tTime;
	CellTaskServer taskServer;
	INetEvent* pINetEvent;
};

#endif