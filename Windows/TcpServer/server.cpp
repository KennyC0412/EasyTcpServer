#include "server.h"
#include "pre.h"
#include "messageHeader.h"
#include "clientsocket.hpp"
#include "CellServer.h"

int TcpServer::initSocket()
{
#ifdef _WIN32
	WORD ver = MAKEWORD(1, 7);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif // _WIN32
	//如果有旧socket存在，关闭它
	if (INVALID_SOCKET != s_sock) {
		closeSocket(s_sock);
	}
	//创建socket
	s_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == s_sock) {
		std::cerr << "Failed to create socket." << std::endl;
		return -1;
	}
	else {
		std::cout << "Create socket successed." << std::endl;
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
		std::cerr << "failed to bind socket:" << s_sock << std::endl;
		return -1;
	}
	else {
		std::cout << "successed to bind socket:" << s_sock << std::endl;
	}
	return 0;
}

int TcpServer::listenPort(int backlog)
{
	if (SOCKET_ERROR == listen(s_sock, backlog)) {
		std::cerr << "failed to listen. socket:" << s_sock << std::endl;
		return -1;
	}
	else {
		std::cout << "successed to listen. socket:" << s_sock << std::endl;
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
			std::cerr << "accept an invalid socket." << std::endl;
			closeSocket(c_sock);
			return -1;
		}
		else {
			addClientToServer(new ClientSocket(c_sock));
			//inet_ntoa(clientAddr.sin_addr);
		}
		return 0;
}

void TcpServer::addClientToServer(ClientSocket* client)
{
	//寻找客户端最少的线程并添加
	auto minServer = g_servers[0];
	for (size_t i = 1; i < g_servers.size(); ++i) {
		if (g_servers[i]->getClientCount() < minServer->getClientCount()) {
			minServer = g_servers[i];
		}
	}
	onJoin(client);
	minServer->addClient(client);
}

bool TcpServer::onRun()
{
	while (isRun()) {
		time4msg();
		fd_set fdRead;
		//fd_set fdWrite;
		//fd_set fdExp;
		//清理集合
		FD_ZERO(&fdRead);
		//FD_ZERO(&fdWrite);
		//FD_ZERO(&fdExp);
		//将描述符加入集合
		FD_SET(s_sock, &fdRead);
		//FD_SET(s_sock, &fdWrite);
		//FD_SET(s_sock, &fdExp);
		//linux下的最大描述符
		SOCKET maxSock = s_sock;
		timeval t{ 0,0 };
		int ret = select(maxSock + 1, &fdRead, nullptr, nullptr, &t);
		if (ret < 0) {
			std::cout << "select finished." << std::endl;
			closeServer();
			return false;
		}
		if (FD_ISSET(s_sock, &fdRead)) {
			//FD_CLR(s_sock, &fdRead);
			acConnection();
			return true;
		}
		return true;
	}
	return false;
}

void TcpServer::Start(int tCount)
{
	for (int n = 0; n < tCount; ++n) {
		auto s = new CellServer(s_sock);
		g_servers.push_back(s);
		//注册网络事件接收对象
		s->setEventObj(this);
		//启动消息处理线程
		s->Start();
	}
}

int TcpServer::sendData(SOCKET c_sock, DataHeader *dh)
{
	if (isRun() && dh) {
		return  send(c_sock, (const char*)dh, dh->dataLength, 0);
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
	auto t1 = tTime.getElapsedSecond();
	if (t1 >= 1.0) {
		std::cout << "thread:<" << g_servers.size() << ">, time: " << "<" << t1 << "> client num:<" <<
			clientNum << ">," << "msgCount:<" << static_cast<int>(msgCount/t1) <<">, recvCount:<" << static_cast<int>(recvCount / t1) << ">" <<std::endl;
		msgCount = 0;
		recvCount = 0;
		tTime.update();
	}
}

void TcpServer::closeServer()
{
#ifdef _WIN32
	for (auto s : g_servers) {
		delete s;
	}
	closesocket(s_sock);
	WSACleanup();
#else
	for (auto s : g_servers) {
		delete s;
	}
	close(s_sock);
#endif
	if (INVALID_SOCKET != s_sock) {
		s_sock = INVALID_SOCKET;
	}
	g_servers.clear();
}

bool TcpServer::isRun()
{
	return s_sock != INVALID_SOCKET;
}

