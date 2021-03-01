#ifndef _SERVER_H_
#define _SERVER_H_

#include "CELLtimestamp.hpp"
#include <algorithm>
#include <atomic>
#include "CELLClient.h"
#include "CELLThread.h"

class CELLServer;

using CELLServerPtr = std::shared_ptr<CELLServer>;

//网络事件接口
class INetEvent
{
public:
	//客户端加入事件
	virtual void onJoin() = 0;
	//客户端离开事件
	virtual void onLeave() = 0;
	//客户端消息事件
	virtual void onNetMsg(CELLServer *, CELLClientPtr &,DataHeader *) = 0;
	virtual void onRecv() = 0;
};


class TcpServer :public INetEvent
{
public:
	TcpServer(int maxClient):s_sock(INVALID_SOCKET),recvCount(0),msgCount(0),clientNum(0),_nMaxClient(maxClient) {}
	virtual ~TcpServer() {  Close(); }
	//创建套接字
	int initSocket();
	//绑定套接字
	int bindSocket(const char* ip, unsigned short);
	//监听端口
	int listenPort(int backlog = BACKLOG);
	//接受连接
	int acConnection();
	//发送数据
	int sendData(SOCKET, DataHeaderPtr&);
	//关闭套接字
	void closeSocket(SOCKET);
	template<typename T>
	void Start(int nThread)
	{
		for (int n = 0; n < nThread; ++n) {
			CELLServerPtr s = std::make_shared<T>(s_sock);
			_servers.push_back(s);
			//注册网络事件接收对象
			s->setEventObj(this);
			//启动消息处理线程
			s->Start();
		}
		_thread.Start(nullptr,
			[this](CELLThread* pThread) { onRun(pThread); }, nullptr);
	}
	//判断
	bool isRun();
	//响应消息
	void time4msg();
	virtual void onRun(CELLThread* pThread) {};
	void Close();
	void addClientToServer(CELLClientPtr);
	//只被一个线程调用 线程安全
	virtual void onJoin() { ++clientNum; }
	//可能会被多个线程调用 线程不安全
	virtual void onLeave() { --clientNum; }
	//可能会被多个线程调用 线程不安全
	virtual inline void onNetMsg(CELLServer *,CELLClientPtr &,DataHeader *) { ++msgCount; }
	void onRecv() { ++recvCount; }
	inline SOCKET getSock() { return s_sock; }
protected:
	//客户端计数
	std::atomic_int clientNum;
	//recv函数计数
	std::atomic_int recvCount;
	//消息包计数
	std::atomic_int msgCount;
private:
	SOCKET s_sock;
	std::vector<CELLServerPtr> _servers;
	//使用std::chrono的消息计时
	CELLTimestamp _tTime;
	CELLThread _thread;
	int _nMaxClient = 10000;
};

#endif // !_SERVER_H_

