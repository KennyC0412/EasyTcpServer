#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <iostream>
#include <thread>
#include "pre.h"
#include "client.h"

void cmdThread(TcpClient* client);

int main()
{
	TcpClient client;
	client.initSocket();
	int retryTimes = 10;
	while (client.connServer(net::IP, net::PORT) < 0 && retryTimes)
		--retryTimes;
	std::thread t(cmdThread, &client);
	t.detach();
	while (client.isRun() ) {
		client.OnRun();
	}
	return 0;
}