#include "CELLEpoll.h"
#include <iostream>


int CELLEpoll::create(int nMaxClient)
{
    int epfd = epoll_create(nMaxClient);
    if(epfd == EPOLL_ERROR){
        CELLLog_Error("failed to create epoll.");
    }
    return epfd;
}

int CELLEpoll::Epoll_ctl(int op,SOCKET sock,uint32_t events){
    //registe epoll event
    epoll_event ev;
    ev.events = events;
    ev.data.fd = sock;
    //registe socket need to listen
    return epoll_ctl(_epfd,op,sock,&ev);
}

int CELLEpoll::wait(int waitTime)
{
    int ret = epoll_wait(_epfd,_pEvents,_maxEvents,waitTime);
    if(EPOLL_ERROR == ret){
        CELLLog_Error("epoll wait.");
    }
    return ret;
}

void CELLEpoll::Close()
{
    if(_epfd > 0){
        close(_epfd);
        _epfd = EPOLL_ERROR;
    }
    if(_pEvents){
        delete []_pEvents;
        _pEvents = nullptr;
    }
}