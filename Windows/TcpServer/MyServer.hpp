#ifndef _MY_SERVER_HPP_
#define _MY_SERVER_HPP_

#include "server.h"
#include "CellServer.h"
#include "CELLLog.h"

//用户自定义server
class MyServer :public TcpServer
{
public:
	//只被一个线程调用 线程安全
	virtual void onJoin(CELLClientPtr& client)
	{
		TcpServer::onJoin(client);
	}
	//可能会被多个线程调用 线程不安全
	virtual void onLeave(CELLClientPtr& client)
	{
		TcpServer::onLeave(client);
	}
	//可能会被多个线程调用 线程不安全
	virtual void onNetMsg(CELLServer* pserver, CELLClientPtr& pclient, DataHeader* dh)
	{
		TcpServer::onNetMsg(pserver, pclient, dh);
		switch (dh->cmd) {
		case CMD_LOGIN: {
			pclient->rstDtHeart();
			DataHeaderPtr ret = std::make_shared<S2C_Heart>();
			//pserver->sendTask(pclient, dynamic_cast<DataHeaderPtr&>(ret));*/
			if (SOCKET_ERROR == pclient->push(ret)) {
				CELLLog::Error("Buffer Full!");
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
			CELLLog::Info("socket :", pclient->getSock(), " receive unknow message. ", dh->dataLength);
		}
		}
	}
	virtual void onRecv(CELLClientPtr&) { ++recvCount; }
};

#endif