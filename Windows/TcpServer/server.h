#ifndef _SERVER_H_
#define _SERVER_H_

#include "messageHeader.h"
#include "pre.h"
#include "CELLtimestamp.hpp"
#include <algorithm>
#include <atomic>
#include "clientsocket.hpp"
#include "CELLTask.h"

class CellServer;

//�����¼��ӿ�
class INetEvent
{
public:
	//�ͻ��˼����¼�
	virtual void onJoin(ClientSocket*) = 0;
	//�ͻ����뿪�¼�
	virtual void onLeave(ClientSocket *) = 0;
	//�ͻ�����Ϣ�¼�
	virtual void onNetMsg(CellServer* pserver,ClientSocket* ,DataHeader*) = 0;
	virtual void onRecv(ClientSocket*) = 0;
};



class TcpServer :public INetEvent
{
public:
	TcpServer():s_sock(INVALID_SOCKET),recvCount(0),msgCount(0),clientNum(0){}
	virtual ~TcpServer() { closeServer(); }
	//�����׽���
	int initSocket();
	//���׽���
	int bindSocket(const char* ip = nullptr, unsigned short = PORT);
	//�����˿�
	int listenPort(int backlog = BACKLOG);
	//��������
	int acConnection();
	//��������
	int sendData(SOCKET,DataHeader *);
	//�ر��׽���
	void closeSocket(SOCKET);
	void Start(int = CELL_SERVER_COUNT);
	//�ж�
	bool isRun();
	//��Ӧ��Ϣ
	void time4msg();
	bool onRun();
	void closeServer();
	void addClientToServer(ClientSocket*);
	//ֻ��һ���̵߳��� �̰߳�ȫ
	virtual void onJoin(ClientSocket*) { ++clientNum; }
	//���ܻᱻ����̵߳��� �̲߳���ȫ
	virtual void onLeave(ClientSocket*) { --clientNum; }
	//���ܻᱻ����̵߳��� �̲߳���ȫ
	virtual void onNetMsg(CellServer* pserver,ClientSocket *,DataHeader*) { ++msgCount; }
	virtual void onRecv(ClientSocket*) { ++recvCount; }
protected:
	//�ͻ��˼���
	std::atomic_int clientNum;
	//recv��������
	std::atomic_int recvCount;
	//��Ϣ������
	std::atomic_int msgCount;
private:
	SOCKET s_sock;
	std::vector<CellServer *> g_servers;
	//ʹ��std::chrono����Ϣ��ʱ
	CELLTimestamp tTime;
};

#endif // !_SERVER_H_

