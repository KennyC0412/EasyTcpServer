#include "preDefine.h"
#include "server.h"
#include <functional>
#include "CellServer.h"
#include <memory>
#include "CELLLog.h"
#include "CELLClient.h"

time_t oldTime = CELLTime::getNowInMilliSec();

void CELLServer::onRun(CELLThread *pThread)
{
	while (pThread->Status()) {
		if (!clientsBuffer.empty())
		{	//���������Ŀͻ��˼��뵽�ͻ�����
			std::lock_guard<std::mutex> lock(_mutex);
			for (auto  c : clientsBuffer) {
				_clients[c->getSock()] = c;
				if (_pINetEvent) {
					_pINetEvent->onJoin();
				}
				ClientJoin(c);
			}
			clientsBuffer.clear();
		}
		if (_clients.empty()) {
			std::chrono::milliseconds t(5);
			std::this_thread::sleep_for(t);
			oldTime = CELLTime::getNowInMilliSec();
			continue;
		}
		if (!core()) {
			pThread->Exit();
			break;
		}
		CheckTime();
	}
}

void CELLServer::CheckTime()
{
	time_t nowTime = CELLTime::getNowInMilliSec();
	time_t t = nowTime - oldTime;
	oldTime = nowTime;
	for (auto iter = _clients.begin(); iter != _clients.end();) {
		//�������
		if (iter->second->checkHeart(t)) {
			close(iter->first);
			ClientLeave(iter->second);
			iter =  _clients.erase(iter);
		}
		else {
			//��ʱ�����������
			iter->second->checkSend(t);
			iter++;
		}
	}
}

void CELLServer::ClientLeave(CELLClientPtr& client)
{
	if (_pINetEvent) {
		_pINetEvent->onLeave();
	}
}

int CELLServer::recvData(CELLClientPtr& client)
{
	//���տͻ�������
	int nLen = client->recvData();
	if (nLen <= 0) {
		return -1;
	}
	//���������������ݼ����¼�
	_pINetEvent->onRecv();
	//ѭ������Ƿ�����Ϣ������
	while (client->hasMsg()) {
		//������Ϣ
		onNetMsg(client, client->front_Msg());
		//�Ƴ�������ͷ����Ϣ
		client->pop_front_Msg();
	}
	return nLen;
}

void CELLServer::onNetMsg(CELLClientPtr& pclient ,DataHeader *dh)
{
    _pINetEvent->onNetMsg(this,pclient,dh);
}

void CELLServer::Start()
{
	//_taskServer.Start();
	//create run destroy
	_thread.Start(nullptr, 
		[this](CELLThread* pThread) {onRun(pThread); },
		[this](CELLThread* pThread) {clearClient(); });
}

void CELLServer::sendTask(CELLClientPtr& pclient,DataHeaderPtr& dh)
{
	/*sendMsg2ClientPtr task = std::make_shared<sendMsg2Client>(pclient,dh);
	_taskServer.addTask(reinterpret_cast<CELLTaskPtr &>(task));*/
}

void CELLServer::addClient(CELLClientPtr client)
{
	std::lock_guard<std::mutex> lock(_mutex);
	clientsBuffer.push_back(client);
}

void CELLServer::Close()
{	
	_taskServer.Close();
	_thread.Close();	
	CELLLog_Info("CellServer Closed.");
}

void CELLServer::clearClient()
{
	for (auto s : _clients) {
		close(s.first);
	}
	close(s_sock);
	if (INVALID_SOCKET != s_sock) {
		s_sock = INVALID_SOCKET;
	}
	_clients.clear();
}
