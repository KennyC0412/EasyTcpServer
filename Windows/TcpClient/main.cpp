#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <iostream>
#include <thread>
#include "pre.h"
#include "client.h"

void cmdThread();
extern bool g_bRun;

int main()
{
	TcpClient * client[cCount];
	for (int i = 0; i < cCount; ++i) {
		client[i] = new TcpClient();
		//client[i]->connServer("127.0.0.1", net::PORT);
		//client[i]->connServer("192.168.124.2", net::PORT);
	}
	for (int i = 0; i < cCount; ++i) {
		if (!g_bRun)
			return 0;
		client[i]->connServer("127.0.0.1", net::PORT);
		std::cout << "Connection = " << i+1 << std::endl;
	}

	std::thread t(cmdThread);
	t.detach();

	Login* login = new Login();
	strcpy(login->userName, "Kenny");
	strcpy(login->passWord, "123456");
	while (g_bRun) {
		for (int i = 0; i < cCount; ++i) {
			//client[i]->onRun();
			client[i]->sendData(login);
		}
	}
	for (int i = 0; i < cCount; ++i) {
		delete client[i];
	}
	getchar();
	return 0;
}