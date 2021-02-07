#include "pre.h"
#include "messageHeader.h"
#include "server.h"

void cmdThread();
extern bool g_bRun;

int main()
{
    TcpServer server;
    server.initSocket();
    server.bindSocket();
    server.listenPort();
    server.Start();
    std::thread t(cmdThread);
    t.detach();
    while (g_bRun)
    {
        server.onRun();
    }
    return 0;
}
