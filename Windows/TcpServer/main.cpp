#include "pre.h"
#include "messageHeader.h"
#include "server.h"
#include "clientsocket.hpp"
#include "CellServer.h"

void cmdThread();
extern bool g_bRun;


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
	virtual void onNetMsg(CellServer* pserver, ClientSocketPtr& pclient, DataHeader * dh)
	{
		TcpServer::onNetMsg(pserver,pclient, dh);
		switch (dh->cmd) {
		case CMD_LOGIN: {
			pclient->rstDtHeart();
			DataHeaderPtr ret = std::make_shared<S2C_Heart>();
			pserver->sendTask(pclient, dynamic_cast<DataHeaderPtr&>(ret));
		}
		break;
		case CMD_LOGOUT:
		{

		}
		break;
		case CMD_HEART_C2S:
		{
			pclient->rstDtHeart();
			DataHeaderPtr ret = std::make_shared<S2C_Heart>();
			pserver->sendTask(pclient, dynamic_cast<DataHeaderPtr&>(ret));
		}
		break;
		default: {
			std::cout << "socket :" << pclient->getSock() << " receive unknow message. " << dh->dataLength << std::endl;
		}
		}
	}
	virtual void onRecv(ClientSocketPtr&) { ++recvCount; }
};

int main()
{
    MyServer server;
    server.initSocket();
    server.bindSocket();
    server.listenPort();
    server.Start();
    std::thread t(cmdThread);
    t.detach();
    while (g_bRun)
    {
        server.onRun();
    }
    return 0;
}

