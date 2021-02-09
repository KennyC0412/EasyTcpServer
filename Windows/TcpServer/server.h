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
//�����¼��ӿ�
class INetEvent 
{
public:
	//�ͻ��˼����¼�
	virtual void onJoin(ClientSocket*) = 0;
	//�ͻ����뿪�¼�
	virtual void onLeave(ClientSocket *) = 0;
	//�ͻ�����Ϣ�¼�
	virtual void onNetMsg(ClientSocket* ,DataHeader*) = 0;
};

class CellServer 
{
public:
	CellServer(SOCKET sock = INVALID_SOCKET) :s_sock(sock), pThread(nullptr),pINetEvent(nullptr){}
	~CellServer() { closeServer(); }
	//����������Ϣ
	bool onRun();
	bool isRun() { return s_sock != INVALID_SOCKET; }
	//��������
	int recvData(ClientSocket* client);
	//��Ӧ��Ϣ
	void onNetMsg(ClientSocket* pclient, DataHeader* dh);
	//�رշ�����
	void closeServer();
	//��ӿͻ���
	void addClient(ClientSocket* client);
	void setEventObj(INetEvent* event) { pINetEvent = event; }
	void Start();
	size_t getClientCount() { return g_clients.size() + clientsBuffer.size(); }
private:
	SOCKET s_sock;
	//���ջ�����
	char szRecv[RECV_BUFF_SIZE] = {};
	//��ʽ�ͻ�����
	std::map<SOCKET,ClientSocket*> g_clients;
	//����ͻ�����
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
	virtual void onNetMsg(ClientSocket *,DataHeader*) { ++recvCount; }
protected:
	//��Ϣ������
	std::atomic_int clientNum;
	std::atomic_int recvCount;
private:
	SOCKET s_sock;
	std::vector<CellServer *> g_servers;
	//ʹ��std::chrono����Ϣ��ʱ
	CELLTimestamp tTime;
};

#endif // !_SERVER_H_

