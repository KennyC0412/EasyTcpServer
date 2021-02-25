#include "pre.h"
#include "messageHeader.h"
#include "server.h"
#include "CELLClient.h"
#include "CellServer.h"
#include "MyServer.hpp"
#include "NetEnvMan.h"

const char* argToStr(int argc, char* argv[], int index, const char* def, const char* argName)
{
	if (index >= argc) {
		CELLLog_Error("argToStr, index=", index, "argv=", argv, argName);
	}
	else {
		def = argv[index];
	}
	CELLLog_Info(argName, "=", def);
	return def;
}

int  argToInt(int argc, char* argv[], int index, int def, const char* argName)
{
	if (index >= argc) {
		CELLLog_Error("argToStr, index=", index, "argv=", argv, argName);
	}
	else {
		def = atoi(argv[index]);
	}
	CELLLog_Info(argName, "=", def);
	return def;
}


int main(int argc,char *argv[])
{

	//const char* strIP = argToStr(argc, argv, 1, "any", "strIP");
	//uint16_t nPort = argToInt(argc, argv, 2,88, "nPort");
	//int nThread = argToInt(argc, argv, 3, 4, "nThread");
	//int nClient = argToInt(argc, argv, 4, 1000, "nClient");
	//
	//if (strcmp(strIP, "any") == 0) {
	//	strIP = nullptr;
	//}

	CELLLog::getInstance().setLogPath("temp\\serverLog", 'w');
    MyServer server;
    server.initSocket();
    server.bindSocket(/*strIP,nPort*/);
    server.listenPort();
    server.Start(/*nThread*/);
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

