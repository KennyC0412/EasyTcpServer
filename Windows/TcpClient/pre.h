#pragma once
#pragma comment(lib,"ws2_32.lib")
#ifndef  _PRE_H_
#define _PRE_H_
#include <vector>
#include <thread>
#include <iostream>
namespace net {
	const short PORT = 8888;
}
const int BACKLOG = 4;
//const char IP{} = "192.168.206.128";
namespace net {
	const char IP[] = "192.168.206.128";
}
const int RECV_BUFF_SIZE = 10240;
const int cCount = 1000;

#endif // ! _PRE_H_
