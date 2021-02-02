#include "client.h"
#include "messageHeader.h"
#include "pre.h"



int TcpClient::initSocket() 
{
#ifdef _WIN32
	//建立windows socket环境
	WORD ver = MAKEWORD(1, 5);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	//如果有旧socket存在，关闭它
	if (INVALID_SOCKET != c_sock) {
		std::cout << "Old socket: " << c_sock << " closed." << std::endl;
		closeSocket();
	}
	//创建socket
	c_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == c_sock) {
		std::cerr << "Failed to create socket." << std::endl;
		return -1;
	}
	else {
		std::cout << "Create socket successed." << std::endl;
	}
	return 0;
}

int TcpClient::connServer(const char *ip,short port)
{
	memset(&_sin, 0, sizeof(_sin));
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(net::PORT);
#ifdef WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	_sin.sin_addr.s_addr = inet_addr(IP);
#endif
	//如果还未创建有效套接字，初始化一个套接字
	if (INVALID_SOCKET == c_sock) {
		std::cout << "No valid socket , new socket initialized." << std::endl;
		initSocket();
	}
	if (-1 == connect(c_sock, (const sockaddr*)&_sin, sizeof(sockaddr_in))) {
		std::cerr << "Failed to connect server." << std::endl;
		return -1;
	}
	else {
		std::cout << "Success to connect server." << std::endl;
		char welcome[128];
		if (0 < recv(c_sock, welcome, 128, 0))
			std::cout << welcome;
	}
	return 0;
}

//接收数据 处理粘包 拆分包
int TcpClient::recvData(SOCKET sock)
{
	char szRecv[1024] = {};
	if (0 >= recv(c_sock, szRecv, sizeof(DataHeader), 0)) {
		std::cout << "Connection closed." << std::endl;
		return -1;
	}
	recv(c_sock, szRecv + sizeof(DataHeader), sizeof(szRecv) - sizeof(DataHeader), 0);
	DataHeader* header = reinterpret_cast<DataHeader*>(szRecv);
	resData(header);
	return 0;
}


void TcpClient::resData(DataHeader* dh)
{
	switch (dh->cmd) {
	case CMD_LOGIN_RESULT:
	{
		LoginResult* loginResult = static_cast<LoginResult*>(dh);
		std::cout << "receive:CMD_LOGIN_RESULT from server. result:" << loginResult->result << "\tdata length:" << loginResult->dataLength << std::endl;
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		LogoutResult* logoutResult = static_cast<LogoutResult*>(dh);
		std::cout << "receive:CMD_LOGOUT_RESULT from server. result:" << logoutResult->result << "\tdata length:" << logoutResult->dataLength << std::endl;
	}
	break;
	case CMD_NEW_USER_JOIN:
	{
		NewUserJoin* newUser = static_cast<NewUserJoin*>(dh);
		std::cout << "receive:CMD_NEW_USER_JOIN from server. New user's socket:" << newUser->sock << "\tdata length:" << newUser->dataLength << std::endl;
	}
	default:
		DataHeader header = { 0,CMD_ERROR };
		send(c_sock, (const char*)&header, sizeof(DataHeader), 0);
	}
}

int TcpClient::sendData(DataHeader *dh)
{
	if (isRun() && dh) {
		return send(c_sock, (const char*)dh, dh->dataLength, 0);
	}
	return SOCKET_ERROR;
}

bool TcpClient::OnRun()
{
	if (isRun()) {
		//初始化
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(c_sock, &fdRead);
		//设置最大标识符
		timeval t{ 2,0 };
		if (0 > select(c_sock + 1, &fdRead, 0, 0, &t)) {
			std::cout << "Socket: " << c_sock << " select task finished." << std::endl;
			return false;
		}
		if (FD_ISSET(c_sock, &fdRead)) {
			FD_CLR(c_sock, &fdRead);
			if (-1 == recvData(c_sock)) {
				std::cout << "Socket: " << c_sock << " select task finished." << std::endl;
				return false;
			}
		}
		return true;
	}
	return false;
}


bool TcpClient::isRun()
{
	return c_sock != INVALID_SOCKET;
}

void TcpClient::closeSocket()
{
#ifdef _WIN32
	closesocket(c_sock);
	c_sock = INVALID_SOCKET;
	//清除windows socket环境
	WSACleanup();
#else
	close(c_sock);
	c_sock = INVALID_SOCKET;
#endif
}

