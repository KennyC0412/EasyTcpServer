#include "pre.h"
#include "messageHeader.h"
#include "server.h"
#include "CELLClient.h"
#include "CellServer.h"
#include "MyServer.hpp"
#include "NetEnvMan.h"


int main()
{
    CELLLog::getInstance().setLogPath("temp\\serverLog.txt", 'w');
    MyServer server;
    server.initSocket();
    server.bindSocket();
    server.listenPort();
    server.Start();
	while (true) {
		char cmdBuf[256];
		std::cin >> cmdBuf;
		if (0 == strcmp(cmdBuf, "exit")) {
			CELLLog::Info("cmd thread quit.");
			break;
		}
		else {
			CELLLog::Info("Wrong command.please input again");
		}
	}
    return 0;
}

