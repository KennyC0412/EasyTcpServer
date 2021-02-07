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
	while (isRun()) {
		if (clientsBuffer.size() > 0) {
			std::lock_guard<std::mutex> lock(m);
			for (auto c : clientsBuffer) {
				g_clients.push_back(c);
			}
			clientsBuffer.clear();
		}
		if (g_clients.empty()) {
			std::chrono::milliseconds t(1);
			std::this_thread::sleep_for(t);
			continue;
		}
		//������socket
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;
		//������
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);
		//�����������뼯��
		FD_SET(s_sock, &fdRead);
		FD_SET(s_sock, &fdWrite);
		FD_SET(s_sock, &fdExp);
		//linux�µ����������
		SOCKET maxSock = g_clients[0]->getSock();
		//��ÿ��ѭ���н��ͻ��˼����������
		size_t size = g_clients.size();
		for (size_t i = 0; i < size; ++i) {
			FD_SET(g_clients[i]->getSock(), &fdRead);
#ifndef _WIN32
			if (maxSock < g_clients[i]->getSock()) {
				maxSock = g_clients[i]->getSock();
			}
#endif 
		}
		timeval t{ 1,0 };
		int ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, &t);
		if (ret < 0) {
			std::cout << "select finished." << std::endl;
			closeServer();
			return false;
		}
		for (size_t i = 0; i < size; ++i) {
			if (FD_ISSET(g_clients[i]->getSock(), &fdRead)) {
				if (-1 == recvData(g_clients[i])) {
					delete g_clients[i];
					auto it = g_clients.begin() + i;
					if (it != g_clients.end()) {
						g_clients.erase(it);
					}
					size--;
				}
			}
		}
	}
	return false;
}

int CellServer::recvData(ClientSocket* client)
{
	int nLen = recv(client->getSock(), szRecv, RECV_BUFF_SIZE, 0);
	if (nLen <= 0) {
		std::cout << "Client quit, connection closed. socket:" << client->getSock() << std::endl;
		return -1;
	}
	//�����յ������ݿ�������Ϣ������
	memcpy(client->msgBuf() + client->getPos(), szRecv, nLen);
	//��Ϣ������ƫ��λ��
	client->setPos(client->getPos() + nLen);
	while (client->getPos() >= sizeof(DataHeader)) {
		DataHeader* header = reinterpret_cast<DataHeader*>(client->msgBuf());
		if (client->getPos() >= header->dataLength) {
			//��¼��������δ�������ݳ���
			int sizeMark = client->getPos() - header->dataLength;
			onNetMsg(header, client->getSock());
			//����������Ϣǰ��
			memcpy(client->msgBuf(), client->msgBuf() + header->dataLength, sizeMark);
			client->setPos(sizeMark);
		}
		else {
			//ʣ����Ϣ���ݲ���һ����Ϣ
			break;
		}
	}
	return 0;
}

void CellServer::onNetMsg(DataHeader* dh, SOCKET c_sock)
{
	recvCount++;
	/*auto t1 = tTime.getElapsedSecond();
	if (t1 >= 1.0) {
		std::cout << "time: " << "<" << t1 << "> client num:<" << g_clients.size() << ">,"
			<< "recvCount:" << recvCount << std::endl;
		recvCount = 0;
		tTime.update();
	}*/
	switch (dh->cmd) {
	case CMD_LOGIN:
	{
		Login* login = static_cast<Login*>(dh);
		//std::cout << "receive:CMD_LOGIN from client socket:" << c_sock << "\tdata length:" << login->dataLength <<
			//"\tuserName :" << login->userName << "\tpassword:" << login->passWord << std::endl;
		//�ж��û���������
		//LoginResult* ret = new LoginResult();
		//send(c_sock, (const char*)ret, sizeof(LoginResult), 0);
	}
	break;
	case CMD_LOGOUT:
	{
		Logout* logout = static_cast<Logout*>(dh);
		//std::cout << "receive:CMD_LOGOUT from client socket:" << c_sock << "\tdata length:" << logout->dataLength <<
			//"\tuserName :" << logout->userName << std::endl;
		//LogoutResult* ret = new LogoutResult();
		//send(c_sock, (const char*)ret, sizeof(LogoutResult), 0);
	}
	break;
	default:
		std::cout << "socket :" << c_sock << " receive unknow message. " << dh->dataLength << std::endl;
		//DataHeader header;
		//send(c_sock, (const char*)&header, sizeof(DataHeader), 0);
	}
}

void CellServer::Start()
{
	pThread = new std::thread(std::mem_fun(&CellServer::onRun),this);
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
		closesocket(s->getSock());
		delete s;
	}
	closesocket(s_sock);
	WSACleanup();
#else
	for (auto s : g_clients) {
		close(s->getSock());
		delete s;
	}
	close(s_sock);
#endif
	if (INVALID_SOCKET != s_sock) {
		s_sock = INVALID_SOCKET;
	}
	g_clients.clear();
}


#endif