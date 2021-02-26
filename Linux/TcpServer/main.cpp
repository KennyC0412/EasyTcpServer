#include "pre.h"
#include "messageHeader.h"
#include "server.h"
#include "CELLClient.h"
#include "CellServer.h"
#include "MyServer.hpp"
#include "NetEnvMan.h"
#include "CELLConfig.hpp"


int main(int argc,char *argv[])
{
	CELLConfig::getInstance().Init(argc, argv);
	const char* strIP = CELLConfig::getInstance().getCMD("strIP", "any");
	uint16_t nPort = CELLConfig::getInstance().getCMD("nPort", 8888);
	int nThread = CELLConfig::getInstance().getCMD("nThread", 4);
	int nMaxClient = CELLConfig::getInstance().getCMD("nClient", 10000);
	
	if (strcmp(strIP, "any") == 0) {
		strIP = nullptr;
	}

	CELLLog::getInstance().setLogPath("./temp/serverLog", 'w');
    MyServer server(nMaxClient);
    server.initSocket();
    server.bindSocket(strIP,nPort);
    server.listenPort();
    server.Start(nThread);
	while (true) {
		char cmdBuf[256];
		std::cin >> cmdBuf;
		if (0 == strcmp(cmdBuf, "exit")) {
			CELLLog_Info("cmd thread quit.");
			break;
		}
		else {
			CELLLog_Warn("Wrong command.please input again");
		}
	}
    return 0;
}

