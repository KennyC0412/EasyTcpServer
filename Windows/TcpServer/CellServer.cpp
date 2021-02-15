#ifdef _WIN32
#pragma once
#endif
#include "pre.h"
#include "server.h"
#include <functional>
#include "CellServer.h"
#include <memory>

void CellServer::onRun()
{
	fd_set fdRead_back;
	bool client_change = true;
	while (isRun()) {
		if (!clientsBuffer.empty()) 
		{	//���������Ŀͻ��˼��뵽�ͻ�����
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
		//linux�µ����������
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
			return;
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
		std::vector<ClientSocketPtr> temp;
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
	return;
}

int CellServer::recvData(ClientSocketPtr& client)
{
	//ֱ��ʹ�û���������������
	char* szRecv = client->msgBuf() + client->getRecvPos();
	int nLen = recv(client->getSock(), szRecv, RECV_BUFF_SIZE-client->getRecvPos(), 0);
	pINetEvent->onRecv(client);
	if (nLen <= 0) {
		return -1;
	}
	//�����յ������ݿ�������Ϣ������
	//memcpy(client->msgBuf() + client->getPos(), szRecv, nLen);
	//��Ϣ������ƫ��λ��
	client->setRecvPos(client->getRecvPos() + nLen);
	while (client->getRecvPos() >= sizeof(DataHeader)) {
		DataHeader *header = reinterpret_cast<DataHeader *>(client->msgBuf());
		if (client->getRecvPos() >= header->dataLength) {
			//��¼��������δ�������ݳ���
			int sizeMark = client->getRecvPos() - header->dataLength;
			onNetMsg(client,header);
			//����������Ϣǰ��
			memcpy(client->msgBuf(), client->msgBuf() + header->dataLength, sizeMark);
			client->setRecvPos(sizeMark);
		}
		else {
			//ʣ����Ϣ���ݲ���һ����Ϣ
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
	//mem_fn�Զ�ʶ��ʹ��ָ������ý��а�
	pThread = new std::thread(std::mem_fn(&CellServer::onRun),this);
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
#ifdef _WIN32
	for (auto s : g_clients) {
		closesocket(s.first);
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


