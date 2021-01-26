#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <iostream>


//#pragma comment(lib,"ws2_32.lib")
const short PORT = 8888;
const int BACKLOG = 4;
int main()
{
    WORD ver = MAKEWORD(1, 0); //版本号
    WSADATA dat;
    WSAStartup(ver, &dat);
    //一个简易TCP服务器
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
    if (bind(s_sock, (sockaddr*)&_sin, sizeof(_sin)) == SOCKET_ERROR) {
        std::cerr << "failed to bind." << std::endl;
    }
    else {
        std::cout << "successed to bind." << std::endl;
    }
    //3.listen监听网络端口
    if (listen(s_sock, BACKLOG) == SOCKET_ERROR) {
        std::cerr << "failed to listen." << std::endl;
    }
    else {
        std::cout << "successed to bind." << std::endl;
    }
    //4.accept接受客户端连接
    sockaddr_in clientAddr;
    SOCKET c_sock = INVALID_SOCKET;
    int addrLen = sizeof(clientAddr);
   
    //5.send向客户端发送数据
    char buffer[] = "Hello, This is server.\n";
    while (true) {
        c_sock = accept(s_sock, (sockaddr*)&clientAddr, &addrLen);
        if (c_sock == INVALID_SOCKET) {
            std::cerr << "invalid socket recived." << std::endl;
        }
        else {
            std::cout << "new client join: IP = " << inet_ntoa(clientAddr.sin_addr) << std::endl;
        }
        send(c_sock, buffer, strlen(buffer) + 1, 0);
    }
   
    //6.关闭套接字
    closesocket(s_sock);

    WSACleanup();
    return 0;
}
