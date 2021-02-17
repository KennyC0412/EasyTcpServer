#pragma once

#ifndef _CELLSERVER_H_
#define _CELLSERVER_H_

#include "pre.h"
#include "CELLTask.h"
#include "clientsocket.hpp"
#include "messageHeader.h"
#include "CELLThread.h"

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
	~CellServer() { Close(); }
	//��������
	int recvData(ClientSocketPtr& client);
	//��Ӧ��Ϣ
	void onNetMsg(ClientSocketPtr& pclient, DataHeader* dh);
	//�رշ�����
	void Close();
	//��ӿͻ���
	void addClient(ClientSocketPtr client);
	void setEventObj(INetEvent* event) { pINetEvent = event; }
	void Start();
	void sendTask(ClientSocketPtr& ,DataHeaderPtr &);
	size_t getClientCount() { return _clients.size() + clientsBuffer.size(); }
	void readData(fd_set&);
	void CheckTime();
	void clearClient();
protected:
	//����������Ϣ
	void onRun(CellThread*);
private:
	CELLTimestamp _tTime;
	CellTaskServer _taskServer;
	INetEvent* pINetEvent;
	SOCKET s_sock;
	//��ʽ�ͻ�����
	std::map<SOCKET, ClientSocketPtr> _clients;
	//����ͻ�����
	std::vector<ClientSocketPtr> clientsBuffer{};
	std::mutex _mutex;
	CellThread _thread;
	bool client_change = true;
};

#endif