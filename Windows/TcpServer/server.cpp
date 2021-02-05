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
			//NewUserJoin userJoin;
			//sendToAll(&userJoin);
			//存储接入客户端的套接字
			g_clients.push_back(new ClientSocket(c_sock));
			std::cout << "new client join: " << inet_ntoa(clientAddr.sin_addr) << ":" 
				<<clientAddr.sin_port<< "\t socket"<<++count<<" = " << c_sock << std::endl;
		}
		return 0;
}

int TcpServer::recvData(ClientSocket *client)
{
	int nLen = recv(client->getSock(), szRecv, RECV_BUFF_SIZE, 0);
	if(nLen <= 0 ){
		std::cout << "Client quit, connection closed. socket:" << client->getSock() << std::endl;
		return -1;
	}
	//将接收到的数据拷贝到消息缓冲区
	memcpy(client->msgBuf() + client->getPos(), szRecv, nLen);
	//消息缓冲区偏移位置
	client->setPos(client->getPos()+nLen);
	while (client->getPos() >= sizeof(DataHeader)) {
		DataHeader* header = reinterpret_cast<DataHeader*>(client->msgBuf());
		if (client->getPos() >= header->dataLength) {
			//记录缓冲区中未处理数据长度
			int sizeMark = client->getPos() - header->dataLength;
			onNetMsg(header, client->getSock());
			//将缓冲区消息前移
			memcpy(client->msgBuf(), client->msgBuf()+header->dataLength, sizeMark);
			client->setPos(sizeMark);
		}
		else {
			//剩余消息数据不够一条消息
			break;
		}
	}
	return 0;
}

void TcpServer::onNetMsg(DataHeader* dh,SOCKET c_sock)
{
	recvCount++;
	auto t1 = tTime.getElapsedSecond();
	if (t1 >= 1.0) {
		std::cout << "time: "<<"<" << t1 << "> client num:<"<<g_clients.size() <<">,"
			<<"recvCount:" << recvCount << std::endl;
		recvCount = 0;
		tTime.update();
	}
	switch (dh->cmd) {
	case CMD_LOGIN:
	{
		Login* login = static_cast<Login*>(dh);
		//std::cout << "receive:CMD_LOGIN from client socket:" << c_sock << "\tdata length:" << login->dataLength <<
			//"\tuserName :" << login->userName << "\tpassword:" << login->passWord << std::endl;
		//判断用户名和密码
		//LoginResult* ret = new LoginResult();
		//send(c_sock, (const char*)ret, sizeof(LoginResult), 0);
	}
	break;
	case CMD_LOGOUT:
	{
		Logout* logout = static_cast<Logout*>(dh);
		//std::cout << "receive:CMD_LOGOUT from client socket:" << c_sock << "\tdata length:" << logout->dataLength <<
			//"\tuserName :" << logout->userName << std::endl;
		//LogoutResult* ret = new LogoutResult();
		//send(c_sock, (const char*)ret, sizeof(LogoutResult), 0);
	}
	break;
	default:
		std::cout << "socket :" << c_sock << " receive unknow message. " << dh->dataLength << std::endl;
		//DataHeader header;
		//send(c_sock, (const char*)&header, sizeof(DataHeader), 0);
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
		sendData(x->getSock(), dh);
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
		int size = g_clients.size();
		for (size_t i = 0; i < size; ++i) {
			FD_SET(g_clients[i]->getSock(), &fdRead);
#ifndef _WIN32
			if (maxSock < g_clients[i]->getSock()) {
				maxSock = g_clients[i]->getSock();
			}
#endif 
		}
		timeval t{ 0,0 };
#ifdef _WIN32
		int ret = select(s_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
#else
		int ret = select(maxSock + 1, &fdRead, &fdWrite, &fdExp, &t);
#endif
		if (ret < 0) {
			std::cout << "select finished." << std::endl;
			closeServer();
			return false;
		}
		if (FD_ISSET(s_sock, &fdRead)){
			FD_CLR(s_sock, &fdRead);
			acConnection();
			return true;
		}
		for (size_t i = 0; i < size; ++i) {
			if (FD_ISSET(g_clients[i]->getSock(), &fdRead)) {
				if (-1 == recvData(g_clients[i])) {
					delete g_clients[i];
					auto it = g_clients.begin() + i;
					if (it != g_clients.end()) {
						g_clients.erase(it);
					}
					size--;
				}
			}
		}
		return true;
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
		closesocket(s->getSock());
		delete s;
	}
	closesocket(s_sock);
	WSACleanup();
#else
	for (auto s : g_clients) {
		close(s->getSock());
		delete s;
	}
	close(s_sock);
#endif
	if (INVALID_SOCKET != s_sock) {
		s_sock = INVALID_SOCKET;
	}
	g_clients.clear();
}

bool TcpServer::isRun()
{
	return s_sock != INVALID_SOCKET;
}