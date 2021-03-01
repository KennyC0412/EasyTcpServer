#ifndef _PRE_H_
#define _PRE_H_
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define EPOLL_ERROR (-1)
#define SOCKET_ERROR (-1)



const int BACKLOG = 5;
//Ԥ���建������С
const int RECV_BUFF_SIZE = 10240 * 5;
const int SEND_BUFF_SIZE = 10240 * 5;

#endif // !1


