#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "pre.h"


int processor(SOCKET c_sock) {
    //缓冲区
    char szRecv[1024] = {};
    //处理接收到的命令
    DataHeader* header = reinterpret_cast<DataHeader*>(szRecv);
    if (recv(c_sock, szRecv, sizeof(DataHeader), 0) <= 0) {
        std::cout << "client has quited." << std::endl;
        return -1;
    }
    switch (header->cmd) {
    case CMD_LOGIN:
    {
        recv(c_sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//DataHeader已被接受，需要偏移DataHeader长度来接受剩余数据
        Login* login = reinterpret_cast<Login*>(szRecv);
        std::cout << "receive:CMD_LOGIN" << "\tdata length:" << login->dataLength <<
            "\tuserName :" << login->userName << "\tpassword:" << login->passWord << std::endl;
        //判断用户名和密码
        LoginResult* ret = new LoginResult();
        send(c_sock, (const char*)ret, sizeof(LoginResult), 0);
    }
    break;
    case CMD_LOGOUT:
    {
        recv(c_sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//DataHeader已被接受，需要偏移DataHeader长度来接受剩余数据
        Logout* logout = reinterpret_cast<Logout*>(szRecv);
        std::cout << "receive:CMD_LOGOUT" << "\tdata length:" << logout->dataLength <<
            "\tuserName :" << logout->userName << std::endl;
        LoginResult* ret = new LoginResult();
        send(c_sock, (const char*)ret, sizeof(LogoutResult), 0);
    }
    break;
    default:
        DataHeader header = { 0,CMD_ERROR };
        send(c_sock, (const char*)&header, sizeof(DataHeader), 0);
    }
    return 0;
}

int main()
{
    WORD ver = MAKEWORD(1, 3); //版本号
    WSADATA dat;
    WSAStartup(ver, &dat);
    //1.新建一个socket
    SOCKET s_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s_sock == INVALID_SOCKET) {
        std::cerr << "failed to create socket." << std::endl;
    }
    //2.绑定接受连接的网络端口
    sockaddr_in _sin;
    memset(&_sin, 0, sizeof(_sin));
    _sin.sin_port = htons(PORT);
    _sin.sin_family = AF_INET;
    _sin.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    if (bind(s_sock, (sockaddr*)&_sin, sizeof(_sin)) == SOCKET_ERROR)
        std::cerr << "failed to bind." << std::endl;
    else
        std::cout << "successed to bind." << std::endl;
    //3.listen监听网络端口
    if (listen(s_sock, BACKLOG) == SOCKET_ERROR)
        std::cerr << "failed to listen." << std::endl;
    else
        std::cout << "successed to listen." << std::endl;
   
    //5.向客户端发送及接受数据
    while (true) {
        //伯克利socket
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExp;

        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExp);

        FD_SET(s_sock, &fdRead);
        FD_SET(s_sock, &fdWrite);
        FD_SET(s_sock, &fdExp);

        //在每轮循环中将客户端加入监听集合
        for (size_t i = 0; i < g_clients.size(); ++i) {
            FD_SET(g_clients[i], &fdRead);
        }

        //使用select对描述符进行监听
       int ret =  select(s_sock + 1, &fdRead, &fdWrite, &fdExp, NULL);
       //没有已监听的描述符，客户端已经退出
       if (ret < 0) {
           std::cout << "select finished." << std::endl;
           break;
       }
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
               //存储接入客户端的套接字
               g_clients.push_back(c_sock);
                std::cout << "new client join: IP = " << inet_ntoa(clientAddr.sin_addr) << "\t socket = " << clientAddr.sin_port << std::endl;
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
