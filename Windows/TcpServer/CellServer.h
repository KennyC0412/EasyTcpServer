#ifndef _CELL_SERVER_H_
#define _CELL_SERVER_H_

#include "CELLTask.h"
#include "CELLtimestamp.hpp"

class CELLServer;
class INetEvent;
using sendMsg2ClientPtr = std::shared_ptr<sendMsg2Client>;
using CELLServerPtr = std::shared_ptr<CELLServer>;
using LoginResultPtr = std::shared_ptr<LoginResult>;
using DataHeaderPtr = std::shared_ptr<DataHeader>;
class CELLServer
{
public:
	CELLServer(SOCKET sock = INVALID_SOCKET) :s_sock(sock), _pINetEvent(nullptr) {}
	~CELLServer() { Close(); }
	//��������
	int recvData(CELLClientPtr& client);
	//��Ӧ��Ϣ
	void onNetMsg(CELLClientPtr& pclient, DataHeader* dh);
	//�رշ�����
	void Close();
	//��ӿͻ���
	void addClient(CELLClientPtr client);
	void setEventObj(INetEvent* event) { _pINetEvent = event; }
	void Start();
	void sendTask(CELLClientPtr& ,DataHeaderPtr &);
	size_t getClientCount() { return _clients.size() + clientsBuffer.size(); }
	void readData(fd_set&);
	void writeData(fd_set&);
	void CheckTime();
	void ClientLeave(CELLClientPtr&);
	void clearClient();
protected:
	//����������Ϣ
	void onRun(CELLThread*);
private:
	CELLTimestamp _tTime;
	CELLTaskServer _taskServer;
	INetEvent* _pINetEvent;
	SOCKET s_sock;
	//��ʽ�ͻ�����
	std::map<SOCKET, CELLClientPtr> _clients;
	//����ͻ�����
	std::vector<CELLClientPtr> clientsBuffer{};
	std::mutex _mutex;
	CELLThread _thread;
	bool client_change = true;
};

#endif