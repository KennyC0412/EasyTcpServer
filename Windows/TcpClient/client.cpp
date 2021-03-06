#include "client.h"
#include "messageHeader.h"
#include "pre.h"
#include "CELLtimestamp.hpp"
#include "NetEnvMan.hpp"
#include "CELLLog.h"


int TcpClient::initSocket() 
{
	NetEnv::init();
	if (_client) {
		CELLLog::Info("close old socket<", _client->getSock(), ">");
		_client.reset();
	}
	//创建socket
	SOCKET c_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == c_sock) {
		CELLLog::Error("Failed to create socket.");
		Close();
		return -1;
	}
	else {
		_client = std::make_shared<CELLClient>(c_sock);
		isConnect = true;
	}
	return c_sock;
}

int TcpClient::connServer(const char *ip,short port)
{	
	//如果还未创建有效套接字，初始化一个套接字
	if ( _client == nullptr) {
		initSocket();
	}
	sockaddr_in _sin;
	memset(&_sin, 0, sizeof(_sin));
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
#ifdef WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	_sin.sin_addr.s_addr = inet_addr(ip);
#endif
	int ret = connect(_client->getSock(), (const sockaddr*)&_sin, sizeof(sockaddr_in));
	if (-1 == ret) {
		CELLLog::Error("Failed to connect server.");
		Close();
	}
	else {
		isConnect == true;
	}
	return ret;
}




int TcpClient::writeData(DataHeaderPtr& dh)
{
	dh->msgID = nSendMsgID++;
	return _client->push(dh);
}



bool TcpClient::onRun(int waitTime)
{
	SOCKET sock = _client->getSock();
	if (isRun()) {
		//初始化
		fdRead.zero();
		fdWrite.zero();
		fdRead.add(sock);
		timeval t{ 0,waitTime };
		int ret = 0;
		if (_client->needWrite()) {
			fdWrite.add(sock);
			ret = select(sock + 1,fdRead.getSet(), fdWrite.getSet(), nullptr, &t);
		}
		else {
			ret = select(sock + 1, fdRead.getSet(), nullptr, nullptr, &t);
		}
		if (ret < 0) {
			CELLLog::Info("Socket: ", sock, " select task finished.");
			Close();
			return false;
		}
		if (fdRead.has(sock)) {
			if (-1 == readData()) {
				CELLLog::Error("Receive fault.");
				Close();
				return false;
			}
		}
		if (fdWrite.has(sock)) {
			if (-1 == _client->sendData()) {
				CELLLog::Error("Write fault.");
				Close();
				return false;
			}
		}
		return true;
	}
	return false;
}

int TcpClient::readData()
{
	int nLen = _client->recvData();
	if (nLen <= 0) {
		return -1;
	}
	while (_client->hasMsg()) {
		onNetMsg(_client->front_Msg());
		_client->pop_front_Msg();
	}
	return nLen;
}

bool TcpClient::isRun()
{
	return _client && isConnect;
}


void TcpClient::Close()
{
	isConnect = false;
}

