#pragma once
#pragma comment(lib,"ws2_32.lib")
#ifndef _PRE_H_
#define _PRE_H_
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>

const short PORT = 8888;
const int BACKLOG = 5;
const int RECV_BUFF_SIZE = 10240;
static int count = 0;
//static int recvCount = 0;
const int CELL_SERVER_COUNT = 4;
#endif // !1


