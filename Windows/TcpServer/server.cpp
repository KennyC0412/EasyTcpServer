#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "pre.h"

int processor(SOCKET c_sock);
std::vector<SOCKET> g_clients;

int main()
{
    WORD ver = MAKEWORD(1, 3); //版本号
    WSADATA dat;
    WSAStartup(ver, &dat);
    //新建一个socket
    SOCKET s_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s_sock == INVALID_SOCKET) {
        std::cerr << "failed to create socket." << std::endl;
    }
    //绑定接受连接的网络端口
    sockaddr_in _sin;
    memset(&_sin, 0, sizeof(_sin));
    _sin.sin_port = htons(PORT);
    _sin.sin_family = AF_INET;
    _sin.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    if (bind(s_sock, (sockaddr*)&_sin, sizeof(_sin)) == SOCKET_ERROR)
        std::cerr << "failed to bind." << std::endl;
    else
        std::cout << "successed to bind." << std::endl;
    //listen监听网络端口
    if (listen(s_sock, BACKLOG) == SOCKET_ERROR)
        std::cerr << "failed to listen." << std::endl;
    else
        std::cout << "successed to listen." << std::endl;
   
    //向客户端发送及接受数据
    while (true) {
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

        //在每轮循环中将客户端加入监听集合
        for (size_t i = 0; i < g_clients.size(); ++i) {
            FD_SET(g_clients[i], &fdRead);
        }

        //使用select对描述符进行监听
        timeval t{ 1,0 };
       int ret =  select(s_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
       //没有已监听的描述符，select任务完成
       if (ret < 0) {
           std::cout << "select finished." << std::endl;
           break;
       }
       //判断描述符是否在集合中，因为select会清理未响应描述符
       if (FD_ISSET(s_sock, &fdRead) ){
           FD_CLR(s_sock, &fdRead);
           //接受客户端连接
           SOCKET c_sock = INVALID_SOCKET;
           sockaddr_in clientAddr;
           int addrLen = sizeof(sockaddr_in);
           c_sock = accept(s_sock, (sockaddr*)&clientAddr, &addrLen);
           if (c_sock == INVALID_SOCKET) {
               std::cerr << "failed to create socket." << std::endl;
           }
           else {
               for (size_t i = 0; i < g_clients.size(); ++i) {
                   NewUserJoin userJoin;
                   userJoin.sock = c_sock;
                   send(g_clients[i],(const char *)&userJoin,sizeof(userJoin),0);
               }
               //存储接入客户端的套接字
               g_clients.push_back(c_sock);
                std::cout << "new client join: IP = " << inet_ntoa(clientAddr.sin_addr) << "\t socket = " << c_sock << std::endl;
                send(c_sock, welcome, strlen(welcome) + 1, 0);
            }
       }
       for (size_t i = 0; i < fdRead.fd_count; ++i) {
           if (-1 == processor(fdRead.fd_array[i])) {
               auto it = std::find(g_clients.begin(),g_clients.end(),fdRead.fd_array[i]);
               if (it != g_clients.end()) {
                   g_clients.erase(it);
               }
           }
       }
    }

    //服务器关闭，关闭所有连接
    std::for_each(g_clients.begin(), g_clients.end(), [](SOCKET s) { closesocket(s); });
    closesocket(s_sock);
    std::cout << "Connect closed." << std::endl;
    WSACleanup();
    return 0;
}
