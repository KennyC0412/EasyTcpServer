#ifndef _CELL_EPOLL_H_
#define _CELL_EPOLL_H_

#if __linux__
#include "preDefine.h"
#include <sys/epoll.h>
#include "CELLClient.h"
#include "CELLLog.h"

class CELLEpoll
{
public:
    CELLEpoll(){}
    CELLEpoll(int nMax):_maxEvents(nMax){
        _epfd = create(nMax);
        _pEvents = new epoll_event[nMax];
    }
    ~CELLEpoll(){Close();}
    //create an epoll object
    int create(int);
    //
    int Epoll_ctl(int,SOCKET,uint32_t);
    int wait(int = 0);
    inline int getfd(){return _epfd;}
    void Close();
    inline epoll_event* getEvent(){return _pEvents;}
private:
    epoll_event *_pEvents = nullptr;
    int _maxEvents;
    int _epfd;
};

#endif
#endif