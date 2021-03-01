#define WIN32_LEAN_AND_MEAN
#include "pre.h"
#include "MsgHeader.h"
#include "server.h"
#include "CELLLog.h"

bool g_bRun = true;

void cmdThread()
{
	while (true) {
		char cmdBuf[256];
		std::cin >> cmdBuf;
		if (0 == strcmp(cmdBuf, "exit")) {
			CELLLog::Info("cmd thread quit.");
			g_bRun = false;
			break;
		}
		else {
			CELLLog::Info("Wrong command.please input again");
		}
	}
}