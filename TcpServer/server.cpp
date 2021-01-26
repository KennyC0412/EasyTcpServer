#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <iostream>

//#pragma comment(lib,"ws2_32.lib")
const short PORT = 8888;
const int BACKLOG = 4;

enum CMD
{
    CMD_LOGIN,
    CMD_LOGOUT,
    CMD_ERROR
};
//DataHead
struct DataHeader
{
    short dataLength;
    short cmd;
};
//DataPackage
struct Login 
{
    char userName[32];
    char passWord[32];
};

struct LoginResult
{
    int result;
};

struct Logout
{
    char userName[32];
};

struct LogoutResult
{
    int result;
};

int main()
{
    WORD ver = MAKEWORD(1, 2); //版本号
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
        std::cout << "successed to listen." << std::endl;
    }
    //4.accept接受客户端连接
    sockaddr_in clientAddr;
    SOCKET c_sock = INVALID_SOCKET;
    int addrLen = sizeof(clientAddr);
   
    //5.send向客户端发送数据
    char welcome[] = "Hello, This is server.\n";
    char r[16] = {};
    char s[1024] = { };

    c_sock = accept(s_sock, (sockaddr*)&clientAddr, &addrLen);
    if (c_sock == INVALID_SOCKET) {
        std::cerr << "invalid socket recived." << std::endl;
    }
    else {
        std::cout << "new client join: IP = " << inet_ntoa(clientAddr.sin_addr) <<"\t socket = " <<clientAddr.sin_port<< std::endl;
        send(c_sock, welcome, strlen(welcome) + 1, 0);
    }
    
    while (true) {
        DataHeader header{};
        if (recv(c_sock, (char *)&header,sizeof(DataHeader), 0) < 0) {
            std::cout << "client has quited." << std::endl;
            closesocket(c_sock);
            break;
          }
        std::cout << "command receive:" << header.cmd << "\tdata length:" << header.dataLength << std::endl;
        switch (header.cmd) {
        case CMD_LOGIN:
        {
            Login login{};
            recv(c_sock, (char*)&login, sizeof(Login), 0);
            //判断用户名和密码
            LoginResult ret{};
            DataHeader dh = { sizeof(LoginResult),CMD_LOGIN };
            send(c_sock, (const char*)&dh, sizeof(DataHeader), 0);
            send(c_sock, (const char*)&ret, sizeof(LoginResult), 0);
        } break;
        case CMD_LOGOUT:
        {
            Logout logout{};
            recv(c_sock, (char*)&logout, sizeof(logout), 0);
            LogoutResult ret{};
            DataHeader dh = { 0,CMD_LOGOUT };
            send(c_sock, (const char*)&dh, sizeof(DataHeader), 0);
            send(c_sock, (const char*)&ret, sizeof(LogoutResult), 0);
        } break;
        default:
            header.cmd = CMD_ERROR;
            header.dataLength = 0;
            send(c_sock, (const char*)&header, sizeof(DataHeader), 0);
        }
    }
    //6.关闭套接字
    closesocket(s_sock);
    std::cout << "Connect closed." << std::endl;
    WSACleanup();
    return 0;
}
