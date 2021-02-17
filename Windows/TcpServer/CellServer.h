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
	//接收数据
	int recvData(ClientSocketPtr& client);
	//响应消息
	void onNetMsg(ClientSocketPtr& pclient, DataHeader* dh);
	//关闭服务器
	void Close();
	//添加客户端
	void addClient(ClientSocketPtr client);
	void setEventObj(INetEvent* event) { pINetEvent = event; }
	void Start();
	void sendTask(ClientSocketPtr& ,DataHeaderPtr &);
	size_t getClientCount() { return _clients.size() + clientsBuffer.size(); }
	void readData(fd_set&);
	void CheckTime();
	void clearClient();
protected:
	//处理网络消息
	void onRun(CellThread*);
private:
	CELLTimestamp _tTime;
	CellTaskServer _taskServer;
	INetEvent* pINetEvent;
	SOCKET s_sock;
	//正式客户队列
	std::map<SOCKET, ClientSocketPtr> _clients;
	//缓冲客户队列
	std::vector<ClientSocketPtr> clientsBuffer{};
	std::mutex _mutex;
	CellThread _thread;
	bool client_change = true;
};

#endif