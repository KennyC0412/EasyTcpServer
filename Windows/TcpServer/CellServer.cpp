#ifdef _WIN32
#pragma once
#endif
#ifndef _CELLSERVER_HPP_
#define _CELLSERVER_HPP_
#include "pre.h"
#include "server.h"
#include <functional>

bool CellServer::onRun()
{
	fd_set fdRead_back;
	bool client_change = true;
	while (isRun()) {
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
				if (maxSock < iter.first ){
					maxSock = iter.first;
				}
			}
			memcpy(&fdRead_back, &fdRead,  sizeof(fd_set));
		}
		else {
			memcpy(&fdRead, &fdRead_back, sizeof(fd_set));
		}
		timeval t{ 0,0 };
		int ret = select(maxSock + 1, &fdRead, nullptr, nullptr, &t);
		if (ret < 0) {
			std::cout << "select finished." << std::endl;
			closeServer();
			return false;
		}
		else if (ret == 0) {
			continue;
		}
#ifdef _WIN32
		for (int i = 0; i < fdRead.fd_count; i++) {
			auto iter = g_clients.find(fdRead.fd_array[i]);
			if (iter != g_clients.end()) {
				if (-1 == recvData(iter->second)) {
					if (pINetEvent)
						pINetEvent->onLeave(iter->second);
					client_change = true;
					g_clients.erase(iter->first);
				}
			}
			else {
				std::cout << "end of iter" <<std::endl;
			}
		}
#else
		std::vector<ClientSocket*> temp;
		for (auto iter : g_clients) {
			if (FD_ISSET(iter.first, &fdRead)) {
				if (-1 == recvData(iter.second)) {
					client_change = true;
					temp.push_back(iter.second);
					if (pINetEvent)
						pINetEvent->onLeave(iter.second);
				}
			}
		}
		for (auto c : temp) {
			g_clients.erase(c->getSock());
			delete c;
		}
#endif
	}
	return false;
}

int CellServer::recvData(ClientSocket* client)
{
	int nLen = recv(client->getSock(), szRecv, RECV_BUFF_SIZE, 0);
	pINetEvent->onRecv(client);
	if (nLen <= 0) {
		return -1;
	}
	//将接收到的数据拷贝到消息缓冲区
	memcpy(client->msgBuf() + client->getPos(), szRecv, nLen);
	//消息缓冲区偏移位置
	client->setPos(client->getPos() + nLen);
	while (client->getPos() >= sizeof(DataHeader)) {
		DataHeader* header = reinterpret_cast<DataHeader*>(client->msgBuf());
		if (client->getPos() >= header->dataLength) {
			//记录缓冲区中未处理数据长度
			int sizeMark = client->getPos() - header->dataLength;
			onNetMsg(client,header);
			//将缓冲区消息前移
			memcpy(client->msgBuf(), client->msgBuf() + header->dataLength, sizeMark);
			client->setPos(sizeMark);
		}
		else {
			//剩余消息数据不够一条消息
			break;
		}
	}
	return 0;
}

void CellServer::onNetMsg(ClientSocket *pclient ,DataHeader* dh)
{
    pINetEvent->onNetMsg(pclient,dh);
	switch (dh->cmd) {
	case CMD_LOGIN:
	{
		Login* login = static_cast<Login*>(dh);
		LoginResult ret;
		pclient->sendData(&ret);
	}
	break;
	case CMD_LOGOUT:
	{
		
	}
	break;
	default: {
		std::cout << "socket :" << pclient->getSock() << " receive unknow message. " << dh->dataLength << std::endl;
	}
	}
}

void CellServer::Start()
{
	//mem_fn自动识别使用指针或引用进行绑定
	pThread = new std::thread(std::mem_fn(&CellServer::onRun),this);
}

void CellServer::addClient(ClientSocket* client)
{
	std::lock_guard<std::mutex> lock(m);
	clientsBuffer.push_back(client);
}

void CellServer::closeServer()
{
#ifdef _WIN32
	for (auto s : g_clients) {
		closesocket(s.first);
		delete s.second;
	}

	closesocket(s_sock);
#else
	for (auto s : g_clients) {
		close(s.first);
		delete s.second;
	}
	close(s_sock);
#endif
	if (INVALID_SOCKET != s_sock) {
		s_sock = INVALID_SOCKET;
	}
	g_clients.clear();
}


#endif