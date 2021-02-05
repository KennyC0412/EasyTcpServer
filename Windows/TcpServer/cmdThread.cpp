#define WIN32_LEAN_AND_MEAN
#include "pre.h"
#include "messageHeader.h"
#include "server.h"

bool g_bRun = true;

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