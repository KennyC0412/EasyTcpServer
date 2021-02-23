#include "server.h"
#include "pre.h"
#include "messageHeader.h"
#include "CELLClient.h"
#include "CellServer.h"
#include "CELLLog.h"
#include "NetEnvMan.h"

int TcpServer::initSocket()
{
	NetEnv::init();
	//如果有旧socket存在，关闭它
	if (INVALID_SOCKET != s_sock) {
		closeSocket(s_sock);
	}
	//创建socket
	s_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == s_sock) {
		CELLLog::Error( "Failed to create socket.",s_sock);
		return -1;
	}
	else {
		CELLLog::Info("Create socket successed.");
	}
	return 0;
}

int TcpServer::bindSocket(const char *ip,unsigned short port )
{
	sockaddr_in _sin;
	memset(&_sin, 0, sizeof(_sin));
	_sin.sin_port = htons(port);
	_sin.sin_family = AF_INET;
#ifdef _WIN32
	if (ip) {
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
	}
	else {
		_sin.sin_addr.S_un.S_addr = INADDR_ANY;
	}
#else
	if(ip) {
		_sin.sin_addr.s_addr = inet_addr(ip);
	}
	else {
		_sin.sin_addr.s_addr = INADDR_ANY;
	}
#endif
	if (INVALID_SOCKET == s_sock) {
		initSocket();
	}
	if (SOCKET_ERROR == bind(s_sock, (const sockaddr*)&_sin, sizeof(_sin))) {
		CELLLog::Error("failed to bind socket:",s_sock);
		return -1;
	}
	else {
		CELLLog::Info("successed to bind socket:",s_sock);
	}
	return 0;
}

int TcpServer::listenPort(int backlog)
{
	if (SOCKET_ERROR == listen(s_sock, backlog)) {
		CELLLog::Error("failed to listen. socket:", s_sock);
		return -1;
	}
	else {
		CELLLog::Info("successed to listen. socket:",s_sock);
	}
	return 0;
}

int TcpServer::acConnection()
{
	sockaddr_in clientAddr;
	memset(&clientAddr, 0, sizeof(sockaddr_in));
	SOCKET c_sock = INVALID_SOCKET;
	int addrLen = sizeof(clientAddr);
#ifdef _WIN32
	c_sock = accept(s_sock, (sockaddr*)&clientAddr, &addrLen);
#else
	c_sock = accept(s_sock, (sockaddr*)&clientAddr, (socklen_t*)&addrLen);
#endif
		if (INVALID_SOCKET == c_sock) {
			CELLLog::Error("accept an invalid socket.");
			closeSocket(c_sock);
			return -1;
		}
		else {
			CELLClientPtr c(new CELLClient(c_sock));
			addClientToServer(c);
			//inet_ntoa(clientAddr.sin_addr);
		}
		return 0;
}

void TcpServer::addClientToServer(CELLClientPtr client)
{
	//寻找客户端最少的线程并添加
	auto minServer = _servers[0];
	for (size_t i = 1; i < _servers.size(); ++i) {
		if (_servers[i]->getClientCount() < minServer->getClientCount()) {
			minServer = _servers[i];
		}
	}
	onJoin(client);
	minServer->addClient(client);
}

void TcpServer::onRun(CELLThread *pThread)
{
	while (pThread->Status()) {
		time4msg();
		fd_set fdRead;
		//清理集合
		FD_ZERO(&fdRead);
		//将描述符加入集合
		FD_SET(s_sock, &fdRead);
		//linux下的最大描述符
		SOCKET maxSock = s_sock;
		timeval t{ 0,0 };
		int ret = select(maxSock + 1, &fdRead, nullptr, nullptr, &t);
		if (ret < 0) {
			CELLLog::Info("Tcp.Server.On.Run.Select.Error.");
			pThread->Exit();
			break;
		}
		if (FD_ISSET(s_sock, &fdRead)) {
			acConnection();
		}
	}
}

void TcpServer::Start(int tCount)
{
	for (int n = 0; n < tCount; ++n) {
		CELLServerPtr s = std::make_shared<CELLServer>(s_sock);
		_servers.push_back(s);
		//注册网络事件接收对象
		s->setEventObj(this);
		//启动消息处理线程
		s->Start();
	}
	_thread.Start(nullptr,
		[this](CELLThread* pThread) { onRun(pThread); }, nullptr);

}

int TcpServer::sendData(SOCKET c_sock, DataHeaderPtr &dh)
{
	if (isRun() && dh) {
		return  send(c_sock, (const char*)dh.get(), dh->dataLength, 0);
	}
	return SOCKET_ERROR;
}

void TcpServer::closeSocket(SOCKET c_sock)
{
#ifdef _WIN32
	closesocket(c_sock);
	c_sock = INVALID_SOCKET;
#else
	close(c_sock);
	c_sock = INVALID_SOCKET;
#endif
}

//计算
void TcpServer::time4msg()
{
	auto t1 = _tTime.getElapsedSecond();
	if (t1 >= 1.0) {
		int num = clientNum;
		CELLLog::Info("thread:< ", _servers.size(), ">, time:<", t1, "> client num:<",num , ">,msgCount:<", static_cast<int>(msgCount / t1), ">, recvCount:<", static_cast<int>(recvCount / t1), ">");
		msgCount = 0;
		recvCount = 0;
		_tTime.update();
	}
}

void TcpServer::Close()
{
	_thread.Close();
	CELLLog::Info("TcpServer Closed.");
#ifdef _WIN32
	closesocket(s_sock);
#else
	close(s_sock);
#endif
	if (INVALID_SOCKET != s_sock) {
		s_sock = INVALID_SOCKET;
	}
}

bool TcpServer::isRun()
{
	return s_sock != INVALID_SOCKET;
}

