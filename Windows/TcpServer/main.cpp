#include "pre.h"
#include "messageHeader.h"
#include "server.h"

void cmdThread();
extern bool g_bRun;


//�û��Զ���server
class MyServer :public TcpServer
{
public:
	//ֻ��һ���̵߳��� �̰߳�ȫ
	virtual void onJoin(ClientSocket*)
	{
		//std::cout << "client join" << std::endl; 
		++clientNum;
	}
	//���ܻᱻ����̵߳��� �̲߳���ȫ
	virtual void onLeave(ClientSocket*)
	{
		//std::cout << "client leave" << std::endl; 
		--clientNum;
	}
	//���ܻᱻ����̵߳��� �̲߳���ȫ
	virtual void onNetMsg(ClientSocket* pclient, DataHeader* dh)
	{
		++recvCount;
		switch (dh->cmd) {
		case CMD_LOGIN:
		{
			//LoginResult ret;
			//pclient->sendData(&ret);
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
    TcpServer server;
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

