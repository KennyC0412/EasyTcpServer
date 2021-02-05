#include "client.h"
#include "messageHeader.h"
#include "pre.h"

class CELLTimestamp {

};

int TcpClient::initSocket() 
{
#ifdef _WIN32
	//����windows socket����
	WORD ver = MAKEWORD(1, 5);
	WSADATA dat;
	WSAStartup(ver, &dat);
#endif
	//����о�socket���ڣ��ر���
	if (INVALID_SOCKET != c_sock) {
		std::cout << "Old socket: " << c_sock << " closed." << std::endl;
		closeSocket();
	}
	//����socket
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
	//�����δ������Ч�׽��֣���ʼ��һ���׽���
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

//�������� ����ճ�� ��ְ�
int TcpClient::recvData(SOCKET sock)
{
	int nLen = recv(c_sock, szRecv, RECV_BUFF_SIZE, 0);
	if (nLen <= 0) {
		std::cout << "Connection closed." << std::endl;
		return -1;
	}
	//�����յ������ݿ�������Ϣ������
	memcpy(szMsgBuf + lastPos, szRecv, nLen);
	//��Ϣ������ƫ��λ��
	lastPos += nLen;
	while (lastPos >= sizeof(DataHeader))
	{
		DataHeader* header = reinterpret_cast<DataHeader*>(szMsgBuf);
		//�ж���Ϣ���������ݳ��ȴ�����Ϣ����
		if (lastPos >= header->dataLength) {
			//��¼��������δ�������ݳ���
			int sizeMark = lastPos - header->dataLength;
			onNetMsg(header);
			//����������Ϣǰ��
			memcpy(szMsgBuf, szMsgBuf + header->dataLength, sizeMark);
			lastPos = sizeMark;
		}
		else {
			//ʣ����Ϣ���ݲ���һ����Ϣ
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
		//��ʼ��
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(c_sock, &fdRead);
		//��������ʶ��
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
	//���windows socket����
	WSACleanup();
#else
	close(c_sock);
	if (INVALID_SOCKET != c_sock)
	c_sock = INVALID_SOCKET;
#endif
}

