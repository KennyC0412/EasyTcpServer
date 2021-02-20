#include "pre.h"
#include "messageHeader.h"
#include "server.h"
#include "CELLClient.h"
#include "CellServer.h"
#include "MyServer.hpp"

void cmdThread();
extern bool g_bRun;


int main()
{
    MyServer server;
    server.initSocket();
    server.bindSocket();
    server.listenPort();
    server.Start();
    std::thread t(cmdThread);
	t.detach();
	while (server.isRun() && g_bRun) {
		std::chrono::milliseconds t(6000);
		std::this_thread::sleep_for(t);
	}
    return 0;
}

