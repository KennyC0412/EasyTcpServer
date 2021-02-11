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
	//处理网络消息
	void onRun();
	bool isRun() { return s_sock != INVALID_SOCKET; }
	//接收数据
	int recvData(ClientSocket* client);
	//响应消息
	void onNetMsg(ClientSocket* pclient, DataHeader* dh);
	//关闭服务器
	void closeServer();
	//添加客户端
	void addClient(ClientSocket* client);
	void setEventObj(INetEvent* event) { pINetEvent = event; }
	void Start();
	void sendTask(ClientSocket *,DataHeader *);
	size_t getClientCount() { return g_clients.size() + clientsBuffer.size(); }
private:
	SOCKET s_sock;
	//正式客户队列
	std::map<SOCKET, ClientSocket*> g_clients;
	//缓冲客户队列
	std::vector<ClientSocket*> clientsBuffer;
	std::mutex m;
	std::thread* pThread;
	CELLTimestamp tTime;
	CellTaskServer taskServer;
	INetEvent* pINetEvent;
};

#endif