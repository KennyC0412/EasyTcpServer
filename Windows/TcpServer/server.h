#ifndef _SERVER_H_
#define _SERVER_H_

#include "messageHeader.h"
#include "pre.h"
#include "CELLtimestamp.hpp"
#include <algorithm>
#include <atomic>
#include "clientsocket.hpp"
#include "CELLThread.h"
class CELLServer;

using ClientSocketPtr = std::shared_ptr<ClientSocket>;
using CELLServerPtr = std::shared_ptr<CELLServer>;

//�����¼��ӿ�
class INetEvent
{
public:
	//�ͻ��˼����¼�
	virtual void onJoin(ClientSocketPtr & ) = 0;
	//�ͻ����뿪�¼�
	virtual void onLeave(ClientSocketPtr &) = 0;
	//�ͻ�����Ϣ�¼�
	virtual void onNetMsg(CELLServer *, ClientSocketPtr &,DataHeader *) = 0;
	virtual void onRecv(ClientSocketPtr &) = 0;
};


class TcpServer :public INetEvent
{
public:
	TcpServer():s_sock(INVALID_SOCKET),recvCount(0),msgCount(0),clientNum(0){}
	virtual ~TcpServer() {  Close(); }
	//�����׽���
	int initSocket();
	//���׽���
	int bindSocket(const char* ip = nullptr, unsigned short = PORT);
	//�����˿�
	int listenPort(int backlog = BACKLOG);
	//��������
	int acConnection();
	//��������
	int sendData(SOCKET, DataHeaderPtr&);
	//�ر��׽���
	void closeSocket(SOCKET);
	void Start(int = CELL_SERVER_COUNT);
	//�ж�
	bool isRun();
	//��Ӧ��Ϣ
	void time4msg();
	void Close();
	void addClientToServer(ClientSocketPtr);
	//ֻ��һ���̵߳��� �̰߳�ȫ
	virtual void onJoin(ClientSocketPtr &) { ++clientNum; }
	//���ܻᱻ����̵߳��� �̲߳���ȫ
	virtual void onLeave(ClientSocketPtr &) { --clientNum; }
	//���ܻᱻ����̵߳��� �̲߳���ȫ
	virtual void onNetMsg(CELLServer *,ClientSocketPtr &,DataHeader *) { ++msgCount; }
	virtual void onRecv(ClientSocketPtr &) { ++recvCount; }
protected:
	void onRun(CELLThread*);
	//�ͻ��˼���
	std::atomic_int clientNum;
	//recv��������
	std::atomic_int recvCount;
	//��Ϣ������
	std::atomic_int msgCount;
private:
	SOCKET s_sock;
	std::vector<CELLServerPtr> _servers;
	//ʹ��std::chrono����Ϣ��ʱ
	CELLTimestamp _tTime;
	CELLThread _thread;
};

#endif // !_SERVER_H_

