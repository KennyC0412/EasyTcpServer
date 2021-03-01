#ifndef _SERVER_H_
#define _SERVER_H_

#include "CELLtimestamp.hpp"
#include <algorithm>
#include <atomic>
#include "CELLClient.h"
#include "CELLThread.h"
#include <vector>

class CELLServer;

using CELLServerPtr = std::shared_ptr<CELLServer>;

//�����¼��ӿ�
class INetEvent
{
public:
	//�ͻ��˼����¼�
	virtual void onJoin() = 0;
	//�ͻ����뿪�¼�
	virtual void onLeave() = 0;
	//�ͻ�����Ϣ�¼�
	virtual void onNetMsg(CELLServer *, CELLClientPtr &,DataHeader *) = 0;
	virtual void onRecv() = 0;
};


class TcpServer :public INetEvent
{
public:
	TcpServer(int maxClient):s_sock(INVALID_SOCKET),recvCount(0),msgCount(0),clientNum(0),_nMaxClient(maxClient) {}
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
	template<typename T>
	void Start(int nThread)
	{
		for (int n = 0; n < nThread; ++n) {
			CELLServerPtr s = std::make_shared<T>(s_sock);
			_servers.push_back(s);
			//ע�������¼����ն���
			s->setEventObj(this);
			//������Ϣ�����߳�
			s->Start();
		}
		_thread.Start(nullptr,
			[this](CELLThread* pThread) { onRun(pThread); }, nullptr);
	}
	//�ж�
	bool isRun();
	//��Ӧ��Ϣ
	void time4msg();
	virtual void onRun(CELLThread* pThread) {};
	void Close();
	void addClientToServer(CELLClientPtr&);
	//ֻ��һ���̵߳��� �̰߳�ȫ
	virtual void onJoin() { ++clientNum; }
	//���ܻᱻ����̵߳��� �̲߳���ȫ
	virtual void onLeave() { --clientNum; }
	//���ܻᱻ����̵߳��� �̲߳���ȫ
	virtual inline void onNetMsg(CELLServer *,CELLClientPtr &,DataHeader *) { ++msgCount; }
	void onRecv() { ++recvCount; }
	inline SOCKET getSock() { return s_sock; }
protected:
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
	int _nMaxClient = 10000;
};

#endif // !_SERVER_H_

