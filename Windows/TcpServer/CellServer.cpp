#ifdef _WIN32
#pragma once
#endif
#include "pre.h"
#include "server.h"
#include <functional>
#include "CellServer.h"
#include <memory>

time_t oldTime = CELLTime::getNowInMilliSec();

void CELLServer::onRun(CELLThread *pThread)
{
	fd_set fdRead_back;
	while (pThread->Status()) {
		if (!clientsBuffer.empty())
		{	//将缓冲区的客户端加入到客户队列
			std::lock_guard<std::mutex> lock(_mutex);
			for (auto c : clientsBuffer) {
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
		fd_set fdRead;
		fd_set fdWrite;
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		//linux下的最大描述符
		SOCKET maxSock = _clients.begin()->first;

		if (client_change) {
			client_change = false;
			for (auto iter : _clients) {
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
		memcpy(&fdWrite, &fdRead_back, sizeof(fd_set));
		timeval t{ 0,1 };
		int ret = select(maxSock + 1, &fdRead,  &fdWrite, nullptr, &t);
		if (ret < 0) {
			std::cout << "CellServer.OnRun.Select.Error" << std::endl;
			pThread->Exit();
			break;
		}
		readData(fdRead);
		writeData(fdWrite);
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
			if (pINetEvent)
				pINetEvent->onLeave(iter->second);
			client_change = true;
			iter->second->destroyObject(iter->second.get());
			iter =  _clients.erase(iter++);
		}
		else {
			//定时发送心跳检测
			//iter->second->checkSend(t);
			iter++;
		}
	}
}

void CELLServer::ClientLeave(ClientSocketPtr client)
{
	if (pINetEvent) {
		pINetEvent->onLeave(client);
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
	for (auto iter : g_clients) {
		if (FD_ISSET(iter.first, &fdRead)) {
			if (-1 == iter->second->sendData()) {
				ClientLeave(iter->second);
				_clients.erase(iter);
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
			std::cout << "end of iter" << std::endl;
		}
	}
#else
	for (auto iter : g_clients) {
		if (FD_ISSET(iter.first, &fdRead)) {
			if (-1 == recvData(iter.second)) {
				ClientLeave(iter->second);
				_clients.erase(iter);
			}
		}
	}

#endif
}

int CELLServer::recvData(ClientSocketPtr& client)
{
	//直接使用缓冲区来接受数据
	char* szRecv = client->msgBuf() + client->getRecvPos();
	int nLen = recv(client->getSock(), szRecv, RECV_BUFF_SIZE-client->getRecvPos(), 0);
	pINetEvent->onRecv(client);
	if (nLen <= 0) {
		return -1;
	}
	//消息缓冲区偏移位置
	client->setRecvPos(client->getRecvPos() + nLen);
	while (client->getRecvPos() >= sizeof(DataHeader)) {
		DataHeader *header = reinterpret_cast<DataHeader *>(client->msgBuf());
		if (client->getRecvPos() >= header->dataLength) {
			//记录缓冲区中未处理数据长度
			int sizeMark = client->getRecvPos() - header->dataLength;
			onNetMsg(client,header);
			//将缓冲区消息前移
			memcpy(client->msgBuf(), client->msgBuf() + header->dataLength, sizeMark);
			client->setRecvPos(sizeMark);
		}
		else {
			//剩余消息数据不够一条消息
			break;
		}
	}
	return 0;
}

void CELLServer::onNetMsg(ClientSocketPtr& pclient ,DataHeader *dh)
{
    pINetEvent->onNetMsg(this,pclient,dh);
}

void CELLServer::Start()
{
	//_taskServer.Start();
	//create run destroy
	_thread.Start(nullptr, 
		[this](CELLThread* pThread) {onRun(pThread); },
		[this](CELLThread* pThread) {clearClient(); });
}

void CELLServer::sendTask(ClientSocketPtr& pclient,DataHeaderPtr& dh)
{
	sendMsg2ClientPtr task = std::make_shared<sendMsg2Client>(pclient,dh);
	_taskServer.addTask(reinterpret_cast<CELLTaskPtr &>(task));
}

void CELLServer::addClient(ClientSocketPtr client)
{
	std::lock_guard<std::mutex> lock(_mutex);
	clientsBuffer.push_back(client);
}

void CELLServer::Close()
{	
	_taskServer.Close();
	_thread.Close();
	std::cout << "CellServer Closed." << std::endl;
}

void CELLServer::clearClient()
{
#ifdef _WIN32
	for (auto s : _clients) {
		closesocket(s.first);
	}
	closesocket(s_sock);
#else
	for (auto s : g_clients) {
		close(s.first);
	}
	close(s_sock);
#endif
	if (INVALID_SOCKET != s_sock) {
		s_sock = INVALID_SOCKET;
	}
	_clients.clear();
}
