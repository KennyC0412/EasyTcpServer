#ifndef _MY_SERVER_H_
#define _MY_SERVER_H_

#include "server.h"
#include "CellServer.h"

//�û��Զ���server
class MyServer :public TcpServer
{
public:
	//ֻ��һ���̵߳��� �̰߳�ȫ
	virtual void onJoin(ClientSocketPtr& client)
	{
		TcpServer::onJoin(client);
	}
	//���ܻᱻ����̵߳��� �̲߳���ȫ
	virtual void onLeave(ClientSocketPtr& client)
	{
		TcpServer::onLeave(client);
	}
	//���ܻᱻ����̵߳��� �̲߳���ȫ
	virtual void onNetMsg(CELLServer* pserver, ClientSocketPtr& pclient, DataHeader* dh)
	{
		TcpServer::onNetMsg(pserver, pclient, dh);
		switch (dh->cmd) {
		case CMD_LOGIN: {
			pclient->rstDtHeart();
			DataHeaderPtr ret = std::make_shared<S2C_Heart>();
			//pserver->sendTask(pclient, dynamic_cast<DataHeaderPtr&>(ret));*/
			if (SOCKET_ERROR == pclient->writeData(ret)) {
				//��������
				//std::cout << "Buffer full\t";
			}
		}
		break;
		case CMD_LOGOUT:
		{

		}
		break;
		case CMD_HEART_C2S:
		{
			/*pclient->rstDtHeart();
			DataHeaderPtr ret = std::make_shared<S2C_Heart>();
			pserver->sendTask(pclient, dynamic_cast<DataHeaderPtr&>(ret));*/
		}
		break;
		default: {
			std::cout << "socket :" << pclient->getSock() << " receive unknow message. " << dh->dataLength << std::endl;
		}
		}
	}
	virtual void onRecv(ClientSocketPtr&) { ++recvCount; }
};

#endif