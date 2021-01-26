#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <iostream>

//#pragma comment(lib,"ws2_32.lib")
const short PORT = 8888;
const int BACKLOG = 4;

int main()
{
	WORD ver = MAKEWORD(1, 0);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//简易TCP客户端
	//1.新建一个socket
	SOCKET c_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (c_sock == INVALID_SOCKET) {
		std::cerr << "failed to create socket." << std::endl;
	}
	//2.连接服务器
	sockaddr_in _sin;
	memset(&_sin, 0, sizeof(_sin));
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(PORT);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (connect(c_sock, (sockaddr*)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		std::cerr << "failed to connect." << std::endl;
	}
	else {
		std::cout << "successed to connect." << std::endl;
	}
		//3.接受服务器信息recv
	char buffer[1024] = {};
	if (recv(c_sock, buffer, 1024, 0) > 0) {
		std::cout << buffer << std::endl;
	}
	//4.关闭closesocket
	closesocket(c_sock);
	WSACleanup();
	getchar();
	return 0;
}