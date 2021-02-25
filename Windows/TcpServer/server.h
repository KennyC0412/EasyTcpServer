#ifndef _SERVER_H_
#define _SERVER_H_

#include "CELLtimestamp.hpp"
#include <algorithm>
#include <atomic>
#include "CELLClient.h"
#include "CELLThread.h"

class CELLServer;

using CELLServerPtr = std::shared_ptr<CELLServer>;

//�����¼��ӿ�
class INetEvent
{
public:
	//�ͻ��˼����¼�
	virtual void onJoin(CELLClientPtr & ) = 0;
	//�ͻ����뿪�¼�
	virtual void onLeave(CELLClientPtr &) = 0;
	//�ͻ�����Ϣ�¼�
	virtual void onNetMsg(CELLServer *, CELLClientPtr &,DataHeader *) = 0;
	virtual void onRecv(CELLClientPtr &) = 0;
};


class TcpServer :public INetEvent
{
public:
	TcpServer():s_sock(INVALID_SOCKET),recvCount(0),msgCount(0),clientNum(0){}
	virtual ~TcpServer() {  Close(); }
	//�����׽���
	int initSocket();
	//���׽���
	int bindSocket(const char* ip, unsigned short);
	//�����˿�
	int listenPort(int backlog = BACKLOG);
	//��������
	int acConnection();
	//��������
	int sendData(SOCKET, DataHeaderPtr&);
	//�ر��׽���
	void closeSocket(SOCKET);
	void Start(int);
	//�ж�
	bool isRun();
	//��Ӧ��Ϣ
	void time4msg();
	void Close();
	void addClientToServer(CELLClientPtr);
	//ֻ��һ���̵߳��� �̰߳�ȫ
	virtual void onJoin(CELLClientPtr &) { ++clientNum; }
	//���ܻᱻ����̵߳��� �̲߳���ȫ
	virtual void onLeave(CELLClientPtr &) { --clientNum; }
	//���ܻᱻ����̵߳��� �̲߳���ȫ
	virtual void onNetMsg(CELLServer *,CELLClientPtr &,DataHeader *) { ++msgCount; }
	virtual void onRecv(CELLClientPtr &) { ++recvCount; }
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

