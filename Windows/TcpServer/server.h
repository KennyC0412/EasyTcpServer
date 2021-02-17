#ifndef _SERVER_H_
#define _SERVER_H_

#include "messageHeader.h"
#include "pre.h"
#include "CELLtimestamp.hpp"
#include <algorithm>
#include <atomic>
#include "clientsocket.hpp"

class CellServer;

using ClientSocketPtr = std::shared_ptr<ClientSocket>;
using CellServerPtr = std::shared_ptr<CellServer>;

//网络事件接口
class INetEvent
{
public:
	//客户端加入事件
	virtual void onJoin(ClientSocketPtr & ) = 0;
	//客户端离开事件
	virtual void onLeave(ClientSocketPtr &) = 0;
	//客户端消息事件
	virtual void onNetMsg(CellServer *, ClientSocketPtr &,DataHeader *) = 0;
	virtual void onRecv(ClientSocketPtr &) = 0;
};


class TcpServer :public INetEvent
{
public:
	TcpServer():s_sock(INVALID_SOCKET),recvCount(0),msgCount(0),clientNum(0){}
	virtual ~TcpServer() { std::cout << "TcpServer Closed" << std::endl; closeServer(); }
	//创建套接字
	int initSocket();
	//绑定套接字
	int bindSocket(const char* ip = nullptr, unsigned short = PORT);
	//监听端口
	int listenPort(int backlog = BACKLOG);
	//接受连接
	int acConnection();
	//发送数据
	int sendData(SOCKET, DataHeaderPtr&);
	//关闭套接字
	void closeSocket(SOCKET);
	void Start(int = CELL_SERVER_COUNT);
	//判断
	bool isRun();
	//响应消息
	void time4msg();
	bool onRun();
	void closeServer();
	void addClientToServer(ClientSocketPtr);
	//只被一个线程调用 线程安全
	virtual void onJoin(ClientSocketPtr &) { ++clientNum; }
	//可能会被多个线程调用 线程不安全
	virtual void onLeave(ClientSocketPtr &) { --clientNum; }
	//可能会被多个线程调用 线程不安全
	virtual void onNetMsg(CellServer *,ClientSocketPtr &,DataHeader *) { ++msgCount; }
	virtual void onRecv(ClientSocketPtr &) { ++recvCount; }
protected:
	//客户端计数
	std::atomic_int clientNum;
	//recv函数计数
	std::atomic_int recvCount;
	//消息包计数
	std::atomic_int msgCount;
private:
	SOCKET s_sock;
	std::vector<CellServerPtr> g_servers;
	//使用std::chrono的消息计时
	CELLTimestamp tTime;
};

#endif // !_SERVER_H_

