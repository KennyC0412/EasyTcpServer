#include "client.h"
#include "messageHeader.h"
#include "pre.h"

class CELLTimestamp {

};

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
	memory = c_sock;
	if (INVALID_SOCKET == c_sock) {
		std::cerr << "Failed to create socket." << std::endl;
		closeSocket();
		return -1;
	}
	else {
		//std::cout << "Create socket successed." << std::endl;
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
		//std::cout << "No valid socket , new socket initialized." << std::endl;
		initSocket();
	}
	if (-1 == connect(c_sock, (const sockaddr*)&_sin, sizeof(sockaddr_in))) {
		std::cerr << "Failed to connect server." << std::endl;
		closeSocket();
		return -1;
	}
	else {
		//std::cout << "Success to connect server:" << ip <<':'<<_sin.sin_port<< std::endl;
	}
	return 0;
}

//接收数据 处理粘包 拆分包
int TcpClient::recvData(SOCKET sock)
{
	int nLen = recv(c_sock, szRecv, RECV_BUFF_SIZE, 0);
	if (nLen <= 0) {
		std::cout << "Connection closed." << std::endl;
		return -1;
	}
	//将接收到的数据拷贝到消息缓冲区
	memcpy(szMsgBuf + lastPos, szRecv, nLen);
	//消息缓冲区偏移位置
	lastPos += nLen;
	while (lastPos >= sizeof(DataHeader))
	{
		DataHeader* header = reinterpret_cast<DataHeader*>(szMsgBuf);
		//判断消息缓冲区数据长度大于消息长度
		if (lastPos >= header->dataLength) {
			//记录缓冲区中未处理数据长度
			int sizeMark = lastPos - header->dataLength;
			onNetMsg(header);
			//将缓冲区消息前移
			memcpy(szMsgBuf, szMsgBuf + header->dataLength, sizeMark);
			lastPos = sizeMark;
		}
		else {
			//剩余消息数据不够一条消息
			break;
		}
	}
	return 0;
}


void TcpClient::onNetMsg(DataHeader* dh)
{
	switch (dh->cmd) {
	case CMD_LOGIN_RESULT:
	{
		LoginResult* loginResult = static_cast<LoginResult*>(dh);
		std::cout << "receive:CMD_LOGIN_RESULT from server result:" << loginResult->result << "\tdata length:" << loginResult->dataLength << std::endl;
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
	}break;
	case CMD_ERROR:
	{
		std::cout << "receive:CMD_ERROR from server. "  << "\tdata length:" << dh->dataLength << std::endl;
	}
	default:
		std::cout << "socket :" <<c_sock <<" receive unknow message. "<< dh->dataLength << std::endl;
	}
}

int TcpClient::sendData(DataHeader *dh)
{
	if (isRun() && dh) {
		return send(c_sock, (const char*)dh, dh->dataLength, 0);
	}
	return SOCKET_ERROR;
}

bool TcpClient::onRun()
{
	if (isRun()) {
		//初始化
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(c_sock, &fdRead);
		//设置最大标识符
		timeval t{ 0,0 };
		if (0 > select(c_sock + 1, &fdRead, 0, 0, &t)) {
			std::cout << "Socket: " << memory << " select task finished." << std::endl;
			closeSocket();
			return false;
		}
		if (FD_ISSET(c_sock, &fdRead)) {
			FD_CLR(c_sock, &fdRead);
			if (-1 == recvData(c_sock)) {
				std::cout << "Socket: " << memory << " select task finished." << std::endl;
				closeSocket();
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
	if(INVALID_SOCKET != c_sock)
	c_sock = INVALID_SOCKET;
	//清除windows socket环境
	WSACleanup();
#else
	close(c_sock);
	if (INVALID_SOCKET != c_sock)
	c_sock = INVALID_SOCKET;
#endif
}

