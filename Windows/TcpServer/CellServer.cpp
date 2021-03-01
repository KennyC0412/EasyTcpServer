#ifdef _WIN32
#pragma once
#endif
#include "pre.h"
#include "server.h"
#include <functional>
#include "CellServer.h"
#include <memory>
#include "CELLLog.h"
#include "CELLClient.h"
#include "FDset.hpp"

time_t oldTime = CELLTime::getNowInMilliSec();

void CELLServer::onRun(CELLThread *pThread)
{
	while (pThread->Status()) {
		if (!clientsBuffer.empty())
		{	//将缓冲区的客户端加入到客户队列
			std::lock_guard<std::mutex> lock(_mutex);
			for (auto  c : clientsBuffer) {
				_clients[c->getSock()] = c;
				if (_pINetEvent) {
					_pINetEvent->onJoin();
				}
			}
			clientsBuffer.clear();
			client_change = true;
		}
		if (_clients.empty()) {
			std::chrono::milliseconds t(5);
			std::this_thread::sleep_for(t);
			oldTime = CELLTime::getNowInMilliSec();
			continue;
		}
		if (!core()) {
			pThread->Exit();
			break;
		}
		CheckTime();
	}
}

void CELLServer::CheckTime()
{
	time_t nowTime = CELLTime::getNowInMilliSec();
	time_t t = nowTime - oldTime;
	oldTime = nowTime;
	for (auto iter = _clients.begin(); iter != _clients.end();) {
		//心跳检测
		if (iter->second->checkHeart(t)) {
#ifdef _WIN32
			closesocket(iter->first);
#else
			close(iter->first);
#endif
			ClientLeave(iter->second);
			iter =  _clients.erase(iter);
		}
		else {
			//定时发送心跳检测
			iter->second->checkSend(t);
			iter++;
		}
	}
}

void CELLServer::ClientLeave(CELLClientPtr& client)
{
	if (_pINetEvent) {
		_pINetEvent->onLeave();
	}
	client_change = true;
	client->destroyObject(client.get());
}



int CELLServer::recvData(CELLClientPtr& client)
{
	//接收客户端数据
	int nLen = client->recvData();
	if (nLen <= 0) {
		return -1;
	}
	//触发接受网络数据计数事件
	_pINetEvent->onRecv();
	//循环检查是否有消息待处理
	while (client->hasMsg()) {
		//处理消息
		onNetMsg(client, client->front_Msg());
		//移除缓冲区头部消息
		client->pop_front_Msg();
	}
	return nLen;
}

void CELLServer::onNetMsg(CELLClientPtr& pclient ,DataHeader *dh)
{
    _pINetEvent->onNetMsg(this,pclient,dh);
}

void CELLServer::Start()
{
	//_taskServer.Start();
	//create run destroy
	_thread.Start(nullptr, 
		[this](CELLThread* pThread) {onRun(pThread); },
		[this](CELLThread* pThread) {clearClient(); });
}

void CELLServer::sendTask(CELLClientPtr& pclient,DataHeaderPtr& dh)
{
	/*sendMsg2ClientPtr task = std::make_shared<sendMsg2Client>(pclient,dh);
	_taskServer.addTask(reinterpret_cast<CELLTaskPtr &>(task));*/
}

void CELLServer::addClient(CELLClientPtr client)
{
	std::lock_guard<std::mutex> lock(_mutex);
	clientsBuffer.push_back(client);
}

void CELLServer::Close()
{	
	_taskServer.Close();
	_thread.Close();	
	CELLLog_Info("CellServer Closed.");
}

void CELLServer::clearClient()
{
#ifdef _WIN32
	for (auto &s : _clients) {
		closesocket(s.first);
	}
	closesocket(s_sock);
#else
	for (auto s : _clients) {
		close(s.first);
	}
	close(s_sock);
#endif
	if (INVALID_SOCKET != s_sock) {
		s_sock = INVALID_SOCKET;
	}
	_clients.clear();
}
