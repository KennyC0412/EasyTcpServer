#ifdef _WIN32
#pragma once
#endif
#include "pre.h"
#include "server.h"
#include <functional>
#include "CellServer.h"
#include <memory>

time_t oldTime = CELLTime::getNowInMilliSec();

void CellServer::onRun()
{
	fd_set fdRead_back;
	while (isRun) {
		if (!clientsBuffer.empty())
		{	//将缓冲区的客户端加入到客户队列
			std::lock_guard<std::mutex> lock(m);
			for (auto c : clientsBuffer) {
				g_clients[c->getSock()] = c;
			}
			clientsBuffer.clear();
			client_change = true;
		}
		if (g_clients.empty()) {
			std::chrono::milliseconds t(5);
			std::this_thread::sleep_for(t);
			oldTime = CELLTime::getNowInMilliSec();
			continue;
		}
		fd_set fdRead;
		FD_ZERO(&fdRead);
		//linux下的最大描述符
		SOCKET maxSock = g_clients.begin()->first;

		if (client_change) {
			client_change = false;
			for (auto iter : g_clients) {
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
		timeval t{ 0,0 };
		int ret = select(maxSock + 1, &fdRead, nullptr, nullptr, &t);
		//if (ret < 0) {
		//	//std::cout << "select finished." << std::endl;
		//	closeServer();
		//	break;
		//}
		readData(fdRead);
		CheckTime();
	}
	sem.wakeup();
}


void CellServer::CheckTime()
{
	time_t nowTime = CELLTime::getNowInMilliSec();
	time_t t = nowTime - oldTime;
	oldTime = nowTime;
	for (auto iter = g_clients.begin(); iter != g_clients.end();) {
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
			iter =  g_clients.erase(iter++);
		}
		else {
			iter->second->checkSend(t);
			iter++;
		}
	}
}

void CellServer::readData(fd_set& fd)
{
#ifdef _WIN32
	for (int i = 0; i < fd.fd_count; i++) {
		auto iter = g_clients.find(fd.fd_array[i]);
		if (iter != g_clients.end()) {
			if (-1 == recvData(iter->second)) {
				closesocket(iter->first);
				if (pINetEvent)
					pINetEvent->onLeave(iter->second);
				client_change = true;
				iter->second->destroyObject(iter->second.get());
				g_clients.erase(iter);
			}
		}
		else {
			std::cout << "end of iter" << std::endl;
		}
	}
#else
	std::vector<ClientSocketPtr> temp;
	for (auto iter : g_clients) {
		if (FD_ISSET(iter.first, &fdRead)) {
			if (-1 == recvData(iter.second)) {
				close(iter->first);
				client_change = true;
				temp.push_back(iter.second);
				if (pINetEvent)
					pINetEvent->onLeave(iter.second);
			}
		}
	}
	for (auto c : temp) {
		g_clients.erase(c->getSock());
	}
#endif
}

int CellServer::recvData(ClientSocketPtr& client)
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

void CellServer::onNetMsg(ClientSocketPtr& pclient ,DataHeader *dh)
{
    pINetEvent->onNetMsg(this,pclient,dh);
}

void CellServer::Start()
{
	//mem_fn自动识别使用指针或引用进行绑定
	std::thread t = std::thread(std::mem_fn(&CellServer::onRun),this);
	isRun = true;
	t.detach();
	taskServer.Start();
}

void CellServer::sendTask(ClientSocketPtr& pclient,DataHeaderPtr& dh)
{
	sendMsg2ClientPtr task = std::make_shared<sendMsg2Client>(pclient,dh);
	taskServer.addTask(reinterpret_cast<CellTaskPtr &>(task));
}

void CellServer::addClient(ClientSocketPtr client)
{
	std::lock_guard<std::mutex> lock(m);
	clientsBuffer.push_back(client);
}

void CellServer::closeServer()
{
	taskServer.Close();
	isRun = false;
	sem.wait();
#ifdef _WIN32
	for (auto s : g_clients) {
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
	g_clients.clear();
}


