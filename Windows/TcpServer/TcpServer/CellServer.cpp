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

time_t oldTime = CELLTime::getNowInMilliSec();

void CELLServer::onRun(CELLThread *pThread)
{
	fd_set fdRead_back;
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
		CheckTime();
		fd_set fdRead{};
		fd_set fdWrite{};
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		//linux下的最大描述符
		SOCKET maxSock = _clients.begin()->first;
		if (client_change) {
			client_change = false;
			for (auto & iter : _clients) {
				FD_SET(iter.first, &fdRead);
				if (maxSock < iter.first) {
					maxSock = iter.first;
				}
			}
			memcpy(&fdRead_back, &fdRead, sizeof(fd_set));
		}
		else {
			memcpy(&fdRead, &fdRead_back, sizeof(fd_set));
		}
		int ret;
		bool NeedWrite = false;
		//检测需要写的客户端
		for (auto& iter : _clients) {
			if (iter.second->needWrite()) {
				NeedWrite = true;
				FD_SET(iter.first, &fdWrite);
			}
		}
		timeval t{ 0,0 };
		if (NeedWrite) {
			ret = select(maxSock + 1, &fdRead, &fdWrite, nullptr, &t);
		}
		else {
			ret = select(maxSock + 1, &fdRead, nullptr, nullptr, &t);
		}
		if (ret < 0) {
			CELLLog_Error("CellServer.OnRun.Select.Error");
			pThread->Exit();
		}
		else if (ret == 0) {
			continue;
		}
		readData(fdRead);
		writeData(fdWrite);
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
			if (_pINetEvent)
				_pINetEvent->onLeave(iter->second);
			client_change = true;
			iter->second->destroyObject(iter->second.get());
			iter =  _clients.erase(iter++);
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

void CELLServer::writeData(fd_set& fdWrite)
{
#ifdef _WIN32
	for (int i = 0; i < fdWrite.fd_count; ++i) {
		auto iter = _clients.find(fdWrite.fd_array[i]);
		if (iter != _clients.end()) {
			if (-1 == iter->second->sendData()) {
				ClientLeave(iter->second);
				_clients.erase(iter);
			}
		}
	}
#else
	for (auto iter : _clients) {
		if (iter.second->needWrite() && FD_ISSET(iter.first, &fdWrite)) {
			if (-1 == iter.second->sendData()) {
				ClientLeave(iter.second);
				 _clients.erase(iter.first);
			}
		}
	}
#endif
}

void CELLServer::readData(fd_set& fdRead)
{
#ifdef _WIN32
	for (int i = 0; i < fdRead.fd_count; i++) {
		auto iter = _clients.find(fdRead.fd_array[i]);
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
		if (FD_ISSET(iter->first, &fdRead)) {
			if (-1 == recvData(iter->second)) {
				ClientLeave(iter->second);
				_clients.erase(iter);
			}
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
