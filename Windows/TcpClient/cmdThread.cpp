#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include "pre.h"

bool RUN = true;

void cmdThread(SOCKET c_sock)
{
	
	while (true) {
		char cmdBuf[256];
		std::cin >> cmdBuf;
		if (0 == strcmp(cmdBuf, "exit")) {
			std::cout << "cmd thread quit." << std::endl;
			RUN = false;
			break;
		}
		else if (0 == strcmp(cmdBuf, "login")) {
			Login login;
			strcpy(login.userName, "Kenny");
			strcpy(login.passWord, "123456");
			send(c_sock, (const char*)&login, sizeof(login), 0);
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			Logout logout;
			strcpy(logout.userName, "Kenny");
			send(c_sock, (const char*)&logout, sizeof(logout), 0);
		}
		else {
			std::cout << "unknown command.please input again:";
		}
	}
}