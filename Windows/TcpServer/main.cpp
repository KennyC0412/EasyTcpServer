#include "pre.h"
#include "messageHeader.h"
#include "server.h"
#include "clientsocket.hpp"
#include "CellServer.h"

void cmdThread();
extern bool g_bRun;


//�û��Զ���server
class MyServer :public TcpServer
{
public:
	//ֻ��һ���̵߳��� �̰߳�ȫ
	virtual void onJoin(ClientSocket* client)
	{
		TcpServer::onJoin(client);
	}
	//���ܻᱻ����̵߳��� �̲߳���ȫ
	virtual void onLeave(ClientSocket* client)
	{
		TcpServer::onLeave(client);
	}
	//���ܻᱻ����̵߳��� �̲߳���ȫ
	virtual void onNetMsg(CellServer* pserver,ClientSocket* pclient, DataHeader* dh)
	{
		TcpServer::onNetMsg(pserver,pclient, dh);
		switch (dh->cmd) {
		case CMD_LOGIN: {
			Login* login = static_cast<Login*>(dh);
			LoginResult *ret = new LoginResult();
			pserver->sendTask(pclient,ret);
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

