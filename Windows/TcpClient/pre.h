#ifndef  _PRE_H_
#define _PRE_H_

#define WIN32_LEAN_AND_MEAN
#ifdef _WIN32
	#pragma comment(lib,"ws2_32.lib")
	#include <Windows.h>
	#include <WinSock2.h>
#else
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <string.h>
	#define SOCKET int
	#define INVALID_SOCKET (SOCKET)(~0)
	#define SOCKET_ERROR				     (-1)
#endif

#include <vector>
#include <thread>
#include <iostream>
#include <atomic>

const short PORT = 8888;
const int BACKLOG = 5;
const int RECV_BUFF_SIZE = 10240 * 5;
const int SEND_BUFF_SIZE = 10240 * 5;
//客户端数量
const int cCount =1000;
//线程数量
const int tCount = 4;

#endif // ! _PRE_H_
