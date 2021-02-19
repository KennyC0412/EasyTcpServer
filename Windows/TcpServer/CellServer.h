#pragma once

#ifndef _CELLSERVER_H_
#define _CELLSERVER_H_

#include "pre.h"
#include "CELLTask.h"
#include "clientsocket.hpp"
#include "messageHeader.h"
#include "CELLThread.h"

class CELLServer;
class INetEvent;
using sendMsg2ClientPtr = std::shared_ptr<sendMsg2Client>;
using CELLServerPtr = std::shared_ptr<CELLServer>;
using LoginResultPtr = std::shared_ptr<LoginResult>;
using DataHeaderPtr = std::shared_ptr<DataHeader>;
class CELLServer
{
public:
	CELLServer(SOCKET sock = INVALID_SOCKET) :s_sock(sock), /*pThread(nullptr)*/ pINetEvent(nullptr) {}
	~CELLServer() { Close(); }
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
	void writeData(fd_set&);
	void CheckTime();
	void ClientLeave(ClientSocketPtr);
	void clearClient();
protected:
	//处理网络消息
	void onRun(CELLThread*);
private:
	CELLTimestamp _tTime;
	CELLTaskServer _taskServer;
	INetEvent* pINetEvent;
	SOCKET s_sock;
	//正式客户队列
	std::map<SOCKET, ClientSocketPtr> _clients;
	//缓冲客户队列
	std::vector<ClientSocketPtr> clientsBuffer{};
	std::mutex _mutex;
	CELLThread _thread;
	bool client_change = true;
};

#endif