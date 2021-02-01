#define WIN32_LEAN_AND_MEAN
#ifdef _WIN32
	#include <windows.h>
	#include <WinSock2.h>
#else 
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <string.h>
	#define SOCKET int
	#define INVALID_SOCKET (SOCKET)(~0)
	#define SOCKET_ERROR (-1)
#endif

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
