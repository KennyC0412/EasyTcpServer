#ifndef _CELL_SERVER_H_
#define _CELL_SERVER_H_

#include "CELLTask.h"
#include "CELLtimestamp.hpp"
#include <map>
#include <vector>

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
	//��������
	int recvData(CELLClientPtr& client);
	//��Ӧ��Ϣ
	void onNetMsg(CELLClientPtr& pclient, DataHeader* dh);
	//�رշ�����
	void Close();
	//���ӿͻ���
	void addClient(CELLClientPtr client);
	void setEventObj(INetEvent* event) { _pINetEvent = event; }
	void Start();
	void sendTask(CELLClientPtr& ,DataHeaderPtr &);
	size_t getClientCount() { return _clients.size() + clientsBuffer.size(); }
	void CheckTime();
	void ClientLeave(CELLClientPtr&);
	virtual void ClientJoin(CELLClientPtr&){};
	void clearClient();
	virtual bool core() = 0;
	inline int getMsg() {
		int temp = msgCount;
		msgCount = 0;
		return temp;
	}
protected:
	//����������Ϣ
	void onRun(CELLThread*);
	int msgCount = 0;
	bool client_change = true;
	//��ʽ�ͻ�����
	std::map<SOCKET, CELLClientPtr> _clients;
private:
	CELLTimestamp _tTime;
	CELLTaskServer _taskServer;
	INetEvent* _pINetEvent;
	SOCKET s_sock;
	//����ͻ�����
	std::vector<CELLClientPtr> clientsBuffer{};
	std::mutex _mutex;
	CELLThread _thread;
};

#endif