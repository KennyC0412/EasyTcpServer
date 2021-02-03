#include "server.h"
#include "pre.h"
#include "messageHeader.h"

int TcpServer::initSocket()
{
#ifdef _WIN32
	WORD ver = MAKEWORD(1, 5);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif // _WIN32
	//如果有旧socket存在，关闭它
	if (INVALID_SOCKET != s_sock) {
		std::cout << "Old socket: " << s_sock << " closed." << std::endl;
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
			return -1;
		}
		else {
			for (size_t i = 0; i < g_clients.size(); ++i) {
				NewUserJoin userJoin;
				userJoin.sock = c_sock;
				send(g_clients[i], (const char*)&userJoin, sizeof(userJoin), 0);
			}
			//存储接入客户端的套接字
			g_clients.push_back(c_sock);
			std::cout << "new client join: " << inet_ntoa(clientAddr.sin_addr) << ":" 
				<<clientAddr.sin_port<< "\t socket = " << c_sock << std::endl;
			send(c_sock, welcome, strlen(welcome) + 1, 0);
		}
		return 0;
}

int TcpServer::recvData(SOCKET c_sock)
{
	char szRecv[1024] = {};
	if (0 >= recv(c_sock, szRecv, sizeof(DataHeader), 0)) {
		std::cout << "Client quit, connection closed. socket:" << c_sock << std::endl;
		closeSocket(c_sock);
		return -1;
	}
	if (INVALID_SOCKET != c_sock) {
		recv(c_sock, szRecv + sizeof(DataHeader), sizeof(szRecv) - sizeof(DataHeader), 0);
		DataHeader* header = reinterpret_cast<DataHeader*>(szRecv);
		resData(header,c_sock);
	}
	return 0;
}

void TcpServer::resData(DataHeader* dh,SOCKET c_sock)
{
	switch (dh->cmd) {
	case CMD_LOGIN:
	{
		Login* login = static_cast<Login*>(dh);
		std::cout << "receive:CMD_LOGIN from client socket:" << c_sock << "\tdata length:" << login->dataLength <<
			"\tuserName :" << login->userName << "\tpassword:" << login->passWord << std::endl;
		//判断用户名和密码
		LoginResult* ret = new LoginResult();
		send(c_sock, (const char*)ret, sizeof(LoginResult), 0);
	}
	break;
	case CMD_LOGOUT:
	{
		Logout* logout = static_cast<Logout*>(dh);
		std::cout << "receive:CMD_LOGOUT from client socket:" << c_sock << "\tdata length:" << logout->dataLength <<
			"\tuserName :" << logout->userName << std::endl;
		LogoutResult* ret = new LogoutResult();
		send(c_sock, (const char*)ret, sizeof(LogoutResult), 0);
	}
	break;
	default:
		DataHeader header = { 0,CMD_ERROR };
		send(c_sock, (const char*)&header, sizeof(DataHeader), 0);
	}
}

int TcpServer::sendData(SOCKET c_sock, DataHeader *dh)
{
	if (isRun() && dh) {
		return  send(c_sock, (const char*)dh, dh->dataLength, 0);
	}
	return SOCKET_ERROR;
}

void TcpServer::sendToAll(DataHeader* dh)
{
	for (auto x : g_clients) {
		sendData(x, dh);
	}
}

bool TcpServer::onRun()
{
	while (isRun()) {
		//伯克利socket
		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExp;
		//清理集合
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExp);
		//将描述符加入集合
		FD_SET(s_sock, &fdRead);
		FD_SET(s_sock, &fdWrite);
		FD_SET(s_sock, &fdExp);
		//linux下的最大描述符
#ifndef _WIN32
		SOCKET maxSock = 0;
#endif 
		//在每轮循环中将客户端加入监听集合
		for (size_t i = 0; i < g_clients.size(); ++i) {
			FD_SET(g_clients[i], &fdRead);
#ifndef _WIN32
			if (maxSock < g_clients[i]) {
				maxSock = g_clients[i];
			}
#endif 
		}
		timeval t{ 0,50 };
#ifdef _WIN32
		int ret = select(s_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
#else
		int ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, &t);
#endif
		if (ret < 0) {
			std::cout << "select finished." << std::endl;
			return false;
		}
		if (FD_ISSET(s_sock, &fdRead)){
			FD_CLR(s_sock, &fdRead);
			acConnection();
		}
		int sockNum = static_cast<int>(g_clients.size());
		for (size_t i = 0; i < sockNum; ++i) {
			if (-1 == recvData(g_clients[i])) {
				auto it = std::find(g_clients.begin(), g_clients.end(), g_clients[i]);
				if (it != g_clients.end()) {
					g_clients.erase(it);
				}
			}
		}
	}
	return false;
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

void TcpServer::closeServer()
{
#ifdef _WIN32
	for (auto s : g_clients) {
		closesocket(s);
	}
	closesocket(s_sock);
	WSACleanup();
#else
	for (auto s : g_clients) {
		close(s);
	}
	close(s_sock);
	if (INVALID_SOCKET != s_sock) {
		s_sock = INVALID_SOCKET;
	}
#endif
}

bool TcpServer::isRun()
{
	return s_sock != INVALID_SOCKET;
}