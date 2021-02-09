#ifndef _SERVER_H_
#define _SERVER_H_

#include "messageHeader.h"
#include "pre.h"
#include "CELLtimestamp.hpp"
#include <algorithm>
#include <atomic>

class ClientSocket
{
public:
	ClientSocket(SOCKET sock):sockfd(sock){}
	~ClientSocket(){}
	SOCKET getSock() { return sockfd; }
	char* msgBuf() { return szMsgBuf; }
	int getPos() { return lastPos; }
	void setPos(int pos) { lastPos = pos; }
	int sendData(DataHeader*);
	SOCKET sockfd;
private:
	char szMsgBuf[RECV_BUFF_SIZE * 5] = {};
	int lastPos = 0;
};
//网络事件接口
class INetEvent 
{
public:
	//客户端加入事件
	virtual void onJoin(ClientSocket*) = 0;
	//客户端离开事件
	virtual void onLeave(ClientSocket *) = 0;
	//客户端消息事件
	virtual void onNetMsg(ClientSocket* ,DataHeader*) = 0;
};

class CellServer 
{
public:
	CellServer(SOCKET sock = INVALID_SOCKET) :s_sock(sock), pThread(nullptr),pINetEvent(nullptr){}
	~CellServer() { closeServer(); }
	//处理网络消息
	bool onRun();
	bool isRun() { return s_sock != INVALID_SOCKET; }
	//接收数据
	int recvData(ClientSocket* client);
	//响应消息
	void onNetMsg(ClientSocket* pclient, DataHeader* dh);
	//关闭服务器
	void closeServer();
	//添加客户端
	void addClient(ClientSocket* client);
	void setEventObj(INetEvent* event) { pINetEvent = event; }
	void Start();
	size_t getClientCount() { return g_clients.size() + clientsBuffer.size(); }
private:
	SOCKET s_sock;
	//接收缓冲区
	char szRecv[RECV_BUFF_SIZE] = {};
	//正式客户队列
	std::map<SOCKET,ClientSocket*> g_clients;
	//缓冲客户队列
	std::vector<ClientSocket*> clientsBuffer;
	std::mutex m;
	std::thread* pThread;
	CELLTimestamp tTime;
	INetEvent* pINetEvent;
};


class TcpServer :public INetEvent
{
public:
	TcpServer():s_sock(INVALID_SOCKET),recvCount(0),clientNum(0){}
	virtual ~TcpServer() { closeServer(); }
	//创建套接字
	int initSocket();
	//绑定套接字
	int bindSocket(const char* ip = nullptr, unsigned short = PORT);
	//监听端口
	int listenPort(int backlog = BACKLOG);
	//接受连接
	int acConnection();
	//发送数据
	int sendData(SOCKET,DataHeader *);
	//关闭套接字
	void closeSocket(SOCKET);
	void Start(int = CELL_SERVER_COUNT);
	//判断
	bool isRun();
	//响应消息
	void time4msg();
	bool onRun();
	void closeServer();
	void addClientToServer(ClientSocket*);
	//只被一个线程调用 线程安全
	virtual void onJoin(ClientSocket*) { ++clientNum; }
	//可能会被多个线程调用 线程不安全
	virtual void onLeave(ClientSocket*) { --clientNum; }
	//可能会被多个线程调用 线程不安全
	virtual void onNetMsg(ClientSocket *,DataHeader*) { ++recvCount; }
protected:
	//消息包计数
	std::atomic_int clientNum;
	std::atomic_int recvCount;
private:
	SOCKET s_sock;
	std::vector<CellServer *> g_servers;
	//使用std::chrono的消息计时
	CELLTimestamp tTime;
};

#endif // !_SERVER_H_

