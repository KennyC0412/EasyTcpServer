#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <iostream>

//#pragma comment(lib,"ws2_32.lib")
const short PORT = 8888;
const int BACKLOG = 4;

int main()
{
	WORD ver = MAKEWORD(1, 1);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//简易TCP客户端
	//新建一个socket
	SOCKET c_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (c_sock == INVALID_SOCKET) {
		std::cerr << "failed to create socket." << std::endl;
	}
	//连接服务器
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
		char buffer[1024] = { };
		if(0 < recv(c_sock, buffer, 1024, 0))
			std::cout << buffer;
	}
	while (true) {
		char s[16] = { };
		char r[1024] = { };
		std::cin >> s;
		if (0 == strcmp(s, "exit")) {
			send(c_sock, s, strlen(s) + 1, 0);
			break;
		} else {
			send(c_sock, s, strlen(s) + 1, 0);
			if (0 < recv(c_sock, r, 1024, 0)) {
				std::cout << r << std::endl;
			}
		}
	}

	//关闭closesocket
	closesocket(c_sock);
	std::cout << "Connect closed." << std::endl;
	WSACleanup();
	Sleep(1000);
	return 0;
}