#include "CELLEpollServer.h"
#include "CELLLog.h"
#include <memory>
bool CELLEpollServer::core()
{
    for(auto iter : _clients){
        if(iter.second->needWrite()){
            if(-1 == _ep.Epoll_ctl(EPOLL_CTL_MOD,iter.second->getSock(),EPOLLIN|EPOLLOUT)){
                CELLLog_Error("CELLEpollServer.Core.ctl1");
            }
        }
        else{
            if(-1 == _ep.Epoll_ctl(EPOLL_CTL_MOD,iter.second->getSock(),EPOLLIN)){
                CELLLog_Error("CELLEpollServer.Core.ctl2");
            }
        }
    }
    int ret = _ep.wait(1);
    if(-1 == ret){
        CELLLog_Error("CELLEpollServer epoll_wait.");
        return false;
    }
    else if(ret == 0){
        return true;
    }
    epoll_event* events = _ep.getEvent();
    for(int i = 0;i < ret;++i){
        auto iter = _clients.find(events[i].data.fd);
        //read
        if(iter != _clients.end()){
            if(events[i].events & EPOLLIN){
                if(SOCKET_ERROR == readData(iter->second)){
                    ClientLeave(iter->second);
                    rmClient(iter->first);
                }
            }
            if(events[i].events & EPOLLOUT){
                if(SOCKET_ERROR == writeData(iter->second)){
                    ClientLeave(iter->second);
                    rmClient(iter->first);
                }
            }
        }
    }
    return true;
}

void CELLEpollServer::ClientJoin(CELLClientPtr& pClient)
{
    _ep.Epoll_ctl(EPOLL_CTL_ADD,pClient->getSock(),EPOLLIN);
}

int CELLEpollServer::readData(CELLClientPtr& pClient)
{
    int ret = recvData(pClient);
    return ret;
}

int CELLEpollServer::writeData(CELLClientPtr& pClient)
{
    int ret = pClient->sendData();
    return ret;
}

void CELLEpollServer::rmClient(SOCKET sock)
{
    _ep.Epoll_ctl(EPOLL_CTL_DEL,sock,EPOLLIN);
    auto iter = _clients.find(sock);
        if(iter != _clients.end()){
            _clients.erase(iter->first);
        }
}

