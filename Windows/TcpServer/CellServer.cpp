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
		_fdRead.zero();
		//linux下的最大描述符
		SOCKET maxSock = _clients.begin()->first;
		if (client_change) {
			client_change = false;
			for (auto & iter : _clients) {
				_fdRead.add(iter.first);
				if (maxSock < iter.first) {
					maxSock = iter.first;
				}
			}
			_fdRead_back.copy(_fdRead);
		}
		else {
			_fdRead.copy(_fdRead_back);
		}
		int ret;
		bool NeedWrite = false;
		_fdWrite.zero();
		//检测需要写的客户端
		for (auto& iter : _clients) {
			if (iter.second->needWrite()) {
				NeedWrite = true;
				_fdWrite.add(iter.first);
			}
		}
		timeval t{ 0,0 };
		if (NeedWrite) {
			ret = select(maxSock + 1, _fdRead.getSet(), _fdWrite.getSet(), nullptr, &t);
		}
		else {
			ret = select(maxSock + 1, _fdRead.getSet(), nullptr, nullptr, &t);
		}
		if (ret < 0) {
			CELLLog_Error("CellServer.OnRun.Select.Error");
			pThread->Exit();
		}
		else if (ret == 0) {
			continue;
		}	
		readData();
		writeData();
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
		_pINetEvent->onLeave(client);
	}
	client_change = true;
	client->destroyObject(client.get());
}

void CELLServer::writeData()
{
#ifdef _WIN32
	fd_set* temp = _fdWrite.getSet();
	for (size_t i = 0; i < temp->fd_count; ++i) {
		auto iter = _clients.find(temp->fd_array[i]);
		if (iter != _clients.end()) {
			if (-1 == iter->second->sendData()) {
				ClientLeave(iter->second);
				_clients.erase(iter);
			}
		}
	}
#else
	for (auto iter = _clients.begin(); iter != _clients.end();) {
		if (iter->second->needWrite() && fdWrite.has(iter->first)){
			if (-1 == iter->second->sendData()) {
				ClientLeave(iter->second);
				iter = _clients.erase(iter);
			}
			if (iter != _clients.end())
				iter++;
		}
		else {
			iter++;
		}
	}
#endif
}

void CELLServer::readData()
{
#ifdef _WIN32
	fd_set* temp = _fdRead.getSet();
	for (size_t i = 0; i < temp->fd_count; i++) {
		auto iter = _clients.find(temp->fd_array[i]);
		if (iter != _clients.end()) {
			if (-1 == recvData(iter->second)) {
				ClientLeave(iter->second);
				_clients.erase(iter);
			}
		}
		else {
			CELLLog_Error("End of iterator");
		}
	}
#else
	for (auto iter = _clients.begin(); iter != _clients.end();) {
		if (fdRead.has(iter->first)) {
			if (-1 == recvData(iter->second)) {
				ClientLeave(iter->second);
				iter = _clients.erase(iter);
			}
			if(iter != _clients.end())
				iter++;
		}
		else {
			iter++;
		}
	}
#endif
}

int CELLServer::recvData(CELLClientPtr& client)
{
	//接收客户端数据
	int nLen = client->recvData();
	if (nLen <= 0) {
		return -1;
	}
	//触发接受网络数据计数事件
	_pINetEvent->onRecv(client);
	//循环检查是否有消息待处理
	while (client->hasMsg()) {
		//处理消息
		++msgCount;
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
