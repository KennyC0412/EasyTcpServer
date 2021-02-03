#include "pre.h"
#include "messageHeader.h"
#include "server.h"


int main()
{
    TcpServer server;
    server.initSocket();
    server.bindSocket();
    server.listenPort();
    server.acConnection();
    
    while (server.isRun())
    {
        server.onRun();
    }
    return 0;
}
