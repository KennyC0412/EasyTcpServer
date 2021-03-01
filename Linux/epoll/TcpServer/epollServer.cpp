#include "epollServer.h"
#include "CELLEpoll.h"
#include "CELLLog.h"
#include "CELLEpollServer.h"

void EpollServer::onRun(CELLThread* pThread)
{
    CELLEpoll ep(1);
    SOCKET sock = getSock();
    if(-1 == ep.Epoll_ctl(EPOLL_CTL_ADD,getSock(),EPOLLIN)){
        CELLLog_Error("EpollServer.OnRun.Epoll_ctl");
    }
    while(pThread->Status()){
        time4msg();
        int ret = ep.wait(1);
        if(ret < 0){
            CELLLog_Error("EpollServer.OnRun.Select.exit");
            pThread->Exit();
            break;
        }
        epoll_event* events = ep.getEvent();
        for(int i = 0;i < ret;++i){
            if(events[i].data.fd == sock){
                if(events[i].events & EPOLLIN){
                    acConnection();
                }
            }
        }
    }
    
}

void EpollServer::Start(int nThread)
{
    TcpServer::Start<CELLEpollServer>(nThread);
}
