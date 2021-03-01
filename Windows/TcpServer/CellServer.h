#ifndef _CELL_SERVER_H_
#define _CELL_SERVER_H_

#include "CELLTask.h"
#include "CELLtimestamp.hpp"
#include "FDset.hpp"

class CELLServer;
class INetEvent;
using CELLServerPtr = std::shared_ptr<CELLServer>;
using LoginResultPtr = std::shared_ptr<LoginResult>;
using DataHeaderPtr = std::shared_ptr<DataHeader>;
class CELLServer
{
public:
	CELLServer(SOCKET sock = INVALID_SOCKET) :s_sock(sock), _pINetEvent(nullptr) {}
	~CELLServer() { Close(); }
	//接收数据
	int recvData(CELLClientPtr& client);
	//响应消息
	void onNetMsg(CELLClientPtr& pclient, DataHeader* dh);
	//关闭服务器
	void Close();
	//添加客户端
	void addClient(CELLClientPtr client);
	void setEventObj(INetEvent* event) { _pINetEvent = event; }
	void Start();
	void sendTask(CELLClientPtr& ,DataHeaderPtr &);
	size_t getClientCount() { return _clients.size() + clientsBuffer.size(); }
	void CheckTime();
	void ClientLeave(CELLClientPtr&);
	void clearClient();
	virtual bool core() = 0;
	inline int getMsg() {
		int temp = msgCount;
		msgCount = 0;
		return temp;
	}
protected:
	//处理网络消息
	void onRun(CELLThread*);
	int msgCount = 0;
	bool client_change = true;
	//正式客户队列
	std::map<SOCKET, CELLClientPtr> _clients;
private:
	CELLTimestamp _tTime;
	CELLTaskServer _taskServer;
	INetEvent* _pINetEvent;
	SOCKET s_sock;
	//缓冲客户队列
	std::vector<CELLClientPtr> clientsBuffer{};
	std::mutex _mutex;
	CELLThread _thread;
};

#endif