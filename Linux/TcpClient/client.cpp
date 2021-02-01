#define WIN32_LEAN_AND_MEAN

#ifdef _WIN32
  #include <windows.h>
  #include <WinSock2.h>
#else
  #include <unistd.h>
  #include <arpa/inet.h>
  #include <string.h>

  #define SOCKET int
  #define INVALID_SOCKET (SOCKET)(~0)
  #define SOCKET_ERROR (-1)
#endif
#include <thread>
#include <iostream>
#include "pre.h"

void cmdThread(SOCKET c_sock);
int processor(SOCKET c_sock);
extern bool RUN;

int main()
{
	SOCKET c_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (c_sock == INVALID_SOCKET) {
		std::cerr << "failed to create socket." << std::endl;
		exit(0);
	}
	sockaddr_in _sin;
	memset(&_sin, 0, sizeof(_sin));
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(PORT);
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
#else
	_sin.sin_addr.s_addr = inet_addr("192.168.124.2");
#endif
	if (connect(c_sock, (sockaddr*)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		std::cerr << "failed to connect." << std::endl;
	}
	else {
		std::cout << "successed to connect." << std::endl;
		char buffer[1024] = { };
		if(0 < recv(c_sock, buffer, 1024, 0))
			std::cout << buffer;
	}

	std::thread t(cmdThread,c_sock);
	t.detach();

	while (RUN) {
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(c_sock,&fdRead);
		timeval t{ 1,0 };
		int ret = select(c_sock + 1, &fdRead, 0, 0, &t);
		if (ret < 0) {
			std::cout << "select finished" << std::endl;
		}
		if (FD_ISSET(c_sock, &fdRead)) {
			FD_CLR(c_sock, &fdRead);
			if (-1 == processor(c_sock)) {
				std::cout << "task finished." << std::endl;
				break;
			}
		}
	}
		
	

	close(c_sock);
	std::cout << "Connect closed." << std::endl;
	return 0;
}
