#ifndef _MY_SERVER_H_
#define _MY_SERVER_H_

#include "server.h"
#include "CellServer.h"

//用户自定义server
class MyServer :public TcpServer
{
public:
	//只被一个线程调用 线程安全
	virtual void onJoin(ClientSocketPtr& client)
	{
		TcpServer::onJoin(client);
	}
	//可能会被多个线程调用 线程不安全
	virtual void onLeave(ClientSocketPtr& client)
	{
		TcpServer::onLeave(client);
	}
	//可能会被多个线程调用 线程不安全
	virtual void onNetMsg(CELLServer* pserver, ClientSocketPtr& pclient, DataHeader* dh)
	{
		TcpServer::onNetMsg(pserver, pclient, dh);
		switch (dh->cmd) {
		case CMD_LOGIN: {
			pclient->rstDtHeart();
			DataHeaderPtr ret = std::make_shared<S2C_Heart>();
			//pserver->sendTask(pclient, dynamic_cast<DataHeaderPtr&>(ret));*/
			if (SOCKET_ERROR == pclient->writeData(ret)) {
				//缓冲区满
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