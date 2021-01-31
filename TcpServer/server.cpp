#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <iostream>

#pragma comment(lib,"ws2_32.lib")
const short PORT = 8888;
const int BACKLOG = 4;

enum CMD
{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_ERROR
};
//DataHead
struct DataHeader
{
    short dataLength;
    short cmd;
};
//DataPackage
struct Login : public DataHeader
{
    Login() {
        dataLength = sizeof(Login);
        cmd = CMD_LOGIN;
    }
    char userName[32];
    char passWord[32];
};

struct LoginResult: public DataHeader
{
    LoginResult() {
        dataLength = sizeof(LoginResult);
        cmd = CMD_LOGIN_RESULT;
        result = 0;
    }
    int result;
};

struct Logout : public DataHeader
{
    Logout() {
        dataLength = sizeof(Logout);
        cmd = CMD_LOGOUT;
    }
    char userName[32];
};

struct LogoutResult : public DataHeader
{
    LogoutResult() {
        dataLength = sizeof(LogoutResult);
        cmd = CMD_LOGOUT_RESULT;
        result = 0;

    }
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
       char welcome[] = "Hello, This is server.\n";
    c_sock = accept(s_sock, (sockaddr*)&clientAddr, &addrLen);
    if (c_sock == INVALID_SOCKET) {
        std::cerr << "invalid socket recived." << std::endl;
    }
    else {
        std::cout << "new client join: IP = " << inet_ntoa(clientAddr.sin_addr) <<"\t socket = " <<clientAddr.sin_port<< std::endl;
        send(c_sock, welcome, strlen(welcome) + 1, 0);
    }

    //5.向客户端发送及接受数据
    DataHeader header{};
    while (true) {
        if (recv(c_sock, (char *)&header,sizeof(DataHeader), 0) <= 0) {
            std::cout << "client has quited." << std::endl;
            closesocket(c_sock);
            break;
          }
        switch (header.cmd) {
        case CMD_LOGIN:
        {
            Login login;
            recv(c_sock, (char*)&login+sizeof(DataHeader), sizeof(login)-sizeof(DataHeader), 0);//DataHeader已被接受，需要偏移DataHeader长度来接受剩余数据
            std::cout << "receive:CMD_LOGIN"  << "\tdata length:" << login.dataLength <<  
            "\tuserName :" << login.userName <<"\tpassword:" << login.passWord << std::endl;
            //判断用户名和密码
            LoginResult ret;
            send(c_sock, (const char*)&ret, sizeof(LoginResult), 0);
        } break;
        case CMD_LOGOUT:
        {
            Logout logout;
            recv(c_sock, (char*)&logout+sizeof(DataHeader), sizeof(logout)-sizeof(DataHeader), 0);//DataHeader已被接受，需要偏移DataHeader长度来接受剩余数据
            std::cout << "receive:CMD_LOGOUT" << "\tdata length:" << logout.dataLength <<
             "\tuserName :" << logout.userName << std::endl;
            LogoutResult ret;
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
