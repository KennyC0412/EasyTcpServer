#ifndef _CELL_EPOLL_SERVER_H_
#define _CELL_EPOLL_SERVER_H_

#include "CellServer.h"
#include "CELLEpoll.h"

class CELLEpollServer: public CELLServer
{
public:
    CELLEpollServer(SOCKET s):_ep(10240),CELLServer(s){}
    bool core();
    virtual void ClientJoin(CELLClientPtr&);
    int readData(CELLClientPtr&);
    int writeData(CELLClientPtr&);
    void rmClient(SOCKET sock);
private:
    std::mutex m;
    CELLEpoll _ep;
};

#endif