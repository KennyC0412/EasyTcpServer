#ifdef _WIN32
	#pragma once
#endif
#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#pragma comment(lib,"ws2_32.lib")
//最大连接数
	#define FD_SETSIZE 10000
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
#ifndef _PRE_H_
#define _PRE_H_
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <map>


const int BACKLOG = 5;
//预定义缓冲区大小
const int RECV_BUFF_SIZE = 10240 * 5;
const int SEND_BUFF_SIZE = 10240 * 5;

#endif // !1


