#define WIN32_LEAN_AND_MEAN
#include "pre.h"
#include "messageHeader.h"
#include "client.h"

void cmdThread(TcpClient *client)
{
	
	while (true) {
		char cmdBuf[256];
		std::cin >> cmdBuf;
		if (0 == strcmp(cmdBuf, "exit")) {
			std::cout << "cmd thread quit." << std::endl;
			client->closeSocket();
			break;
		}
		else if (0 == strcmp(cmdBuf, "login")) {
			Login login;
			strcpy(login.userName, "Kenny");
			strcpy(login.passWord, "123456");
			client->sendData(&login);
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			Logout logout;
			strcpy(logout.userName, "Kenny");
			client->sendData(&logout);
		}
		else {
			std::cout << "Wrong command.please input again:";
		}
	}
}