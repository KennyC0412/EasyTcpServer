#ifndef _EPOLL_SERVER_H_
#define _EPOLL_SERVER_H_

#include "server.h"

class EpollServer:public TcpServer
{
public:
    EpollServer(int maxClient):TcpServer(maxClient){};
    virtual void onRun(CELLThread* pThread);
    void Start(int nThread);
private:

};

#endif