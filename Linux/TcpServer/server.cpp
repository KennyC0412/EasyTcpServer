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

#include <iostream>
#include <vector>
#include <algorithm>
#include "pre.h"

int processor(SOCKET c_sock);
std::vector<SOCKET> g_clients;

int main()
{
    SOCKET s_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (s_sock == INVALID_SOCKET) {
        std::cerr << "failed to create socket." << std::endl;
    }
    sockaddr_in _sin;
    memset(&_sin,0,sizeof(_sin));
    _sin.sin_port = htons(PORT);
    _sin.sin_family = AF_INET;
    _sin.sin_addr.s_addr = INADDR_ANY;
    if (bind(s_sock, (sockaddr*)&_sin, sizeof(_sin)) == SOCKET_ERROR)
        std::cerr << "failed to bind." << std::endl;
    else
        std::cout << "successed to bind." << std::endl;
    if (listen(s_sock, BACKLOG) == SOCKET_ERROR)
        std::cerr << "failed to listen." << std::endl;
    else
        std::cout << "successed to listen." << std::endl;
   
    while (true) {
	
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExp;

        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExp);
	
	FD_SET(s_sock, &fdRead);
        FD_SET(s_sock, &fdWrite);
        FD_SET(s_sock, &fdExp);
	SOCKET maxSock = s_sock;
        for (size_t i = 0; i < g_clients.size(); ++i) {
            FD_SET(g_clients[i], &fdRead);
		if(maxSock < g_clients[i]){
			maxSock = g_clients[i];
		}
        }

        timeval t{ 3,0 };
       int ret =  select(maxSock+1, &fdRead, &fdWrite, &fdExp, &t);
       if (ret < 0) {
           std::cout << "select finished." << std::endl;
           break;
       }
       if (FD_ISSET(s_sock, &fdRead) ){
           FD_CLR(s_sock, &fdRead);
           SOCKET c_sock = INVALID_SOCKET;
           sockaddr_in clientAddr;
           int addrLen = sizeof(sockaddr_in);
           c_sock = accept(s_sock, (sockaddr*)&clientAddr,(socklen_t*)&addrLen);
           if (c_sock == INVALID_SOCKET) {
               std::cerr << "failed to create socket." << std::endl;
           }
           else {
               g_clients.push_back(c_sock);
                std::cout << "new client join: IP = " << inet_ntoa(clientAddr.sin_addr) << "\t socket = " << clientAddr.sin_port << std::endl;
                send(c_sock, welcome, strlen(welcome) + 1, 0);
            }
       }
	if(g_clients.size()){
		for(size_t i = 0; i < g_clients.size(); ++i){
			if(FD_ISSET(g_clients[i],&fdRead)){
				if(-1 == processor(g_clients[i])){
					auto it = std::find(g_clients.begin(),g_clients.end(),g_clients[i]);
					if(it != g_clients.end())
					g_clients.erase(it);
				}
			}
		}
	}	 
    }

    std::for_each(g_clients.begin(), g_clients.end(), [](SOCKET s) { close(s); });
    close(s_sock);
    std::cout << "Connect closed." << std::endl;
    return 0;
}
