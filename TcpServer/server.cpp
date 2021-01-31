#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include "pre.h"


int main()
{
    WORD ver = MAKEWORD(1, 2); //�汾��
    WSADATA dat;
    WSAStartup(ver, &dat);
    //1.�½�һ��socket
    SOCKET s_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s_sock == INVALID_SOCKET) {
        std::cerr << "failed to create socket." << std::endl;
    }
    //2.�󶨽������ӵ�����˿�
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
    //3.listen��������˿�
    if (listen(s_sock, BACKLOG) == SOCKET_ERROR) {
        std::cerr << "failed to listen." << std::endl;
    }
    else {
        std::cout << "successed to listen." << std::endl;
    }
    //4.accept���ܿͻ�������
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

    //5.��ͻ��˷��ͼ���������
    while (true) {
        char szRecv[1024] = {};
        //δ�յ���Ч��Ϣ���ͻ����Ѿ��˳�
        if (recv(c_sock, szRecv,sizeof(DataHeader), 0) <= 0) {
            std::cout << "client has quited." << std::endl;
            closesocket(c_sock);
            break;
          }
        //������յ�������
        DataHeader* header = reinterpret_cast<DataHeader*>(szRecv);
        switch (header->cmd) {
        case CMD_LOGIN:
        {
            recv(c_sock, szRecv+sizeof(DataHeader), header->dataLength-sizeof(DataHeader), 0);//DataHeader�ѱ����ܣ���Ҫƫ��DataHeader����������ʣ������
            Login* login = reinterpret_cast<Login*>(szRecv);
            std::cout << "receive:CMD_LOGIN"  << "\tdata length:" << login->dataLength <<  
            "\tuserName :" << login->userName <<"\tpassword:" << login->passWord << std::endl;
            //�ж��û���������
            LoginResult *ret = new LoginResult();
            send(c_sock, (const char*)ret, sizeof(LoginResult), 0);
        } 
        break;
        case CMD_LOGOUT:
        {
            recv(c_sock, szRecv+sizeof(DataHeader), header->dataLength-sizeof(DataHeader), 0);//DataHeader�ѱ����ܣ���Ҫƫ��DataHeader����������ʣ������
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
    }
    //6.�ر��׽���
    closesocket(s_sock);
    std::cout << "Connect closed." << std::endl;
    WSACleanup();
    return 0;
}
