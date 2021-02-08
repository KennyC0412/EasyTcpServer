#define WIN32_LEAN_AND_MEAN
#include "pre.h"
#include "messageHeader.h"
#include "client.h"

bool g_bRun = true;
extern TcpClient* client[cCount];

void cmdThread()
{
	
	while (true) {
		char cmdBuf[256];
		std::cin >> cmdBuf;
		if (0 == strcmp(cmdBuf, "exit")) {
			std::cout << "cmd thread quit." << std::endl;
			g_bRun = false;
			break;
		}
		else {
			std::cout << "Wrong command.please input again:";
		}
	}
}

void sendThread(int id)
{
	int c = cCount / tCount;
	int begin = (id - 1) * c;
	int end = id * c;
	for (int i = begin; i < end; ++i) {
		client[i] = new TcpClient();
	}
	for (int i = begin; i < end; ++i) {
		client[i]->connServer("127.0.0.1", net::PORT);
	}
	std::cout << "thread<" << id <<">,Connect = <begin:" << begin<<",end:"<<end<<">" <<std::endl;
	std::chrono::milliseconds t(3000);
	std::this_thread::sleep_for(t);

	Login  login[1];
	for (int i = 0; i < 1; ++i) {
		strcpy(login[i].userName, "Kenny");
		strcpy(login[i].passWord, "123456");
	}
	const int len = sizeof(login);
	while (g_bRun) {
		for (int i = begin; i < end; ++i) {
			client[i]->sendData(login,len);
		}
	}
	for (int i = begin; i < end; ++i) {
		client[i]->closeSocket();
		delete client[i];
	}
}