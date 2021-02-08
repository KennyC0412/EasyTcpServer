#pragma once
#pragma comment(lib,"ws2_32.lib")
#ifndef _PRE_H_
#define _PRE_H_
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>

//端口
const short PORT = 8888;
//监听队列
const int BACKLOG = 5;
//预定义缓冲区大小
const int RECV_BUFF_SIZE = 10240;
//服务器线程数
const int CELL_SERVER_COUNT = 4;
#endif // !1


