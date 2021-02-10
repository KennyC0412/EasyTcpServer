#include "pre.h"
#include "messageHeader.h"
#include "server.h"

void cmdThread();
extern bool g_bRun;


//用户自定义server
class MyServer :public TcpServer
{
public:
	//只被一个线程调用 线程安全
	virtual void onJoin(ClientSocket*)
	{
		++clientNum;
	}
	//可能会被多个线程调用 线程不安全
	virtual void onLeave(ClientSocket*)
	{
		--clientNum;
	}
	//可能会被多个线程调用 线程不安全
	virtual void onNetMsg(ClientSocket* pclient, DataHeader* dh)
	{
		++msgCount;
		switch (dh->cmd) {
		case CMD_LOGIN:
		{
			Login* login = static_cast<Login*>(dh);
			/*LoginResult ret;
			pclient->sendData(&ret);*/
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

