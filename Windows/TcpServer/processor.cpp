#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "pre.h"

int processor(SOCKET c_sock) {
    //������
    char szRecv[1024] = {};
    //������յ�������
    DataHeader* header = reinterpret_cast<DataHeader*>(szRecv);
    if (recv(c_sock, szRecv, sizeof(DataHeader), 0) <= 0) {
        std::cout << "client has quited. socket is:" << c_sock << std::endl;
        return -1;
    }
    switch (header->cmd) {
    case CMD_LOGIN:
    {
        recv(c_sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//DataHeader�ѱ����ܣ���Ҫƫ��DataHeader����������ʣ������
        Login* login = reinterpret_cast<Login*>(szRecv);
        std::cout << "receive:CMD_LOGIN from client socket:"<< c_sock << "\tdata length:" << login->dataLength <<
            "\tuserName :" << login->userName << "\tpassword:" << login->passWord << std::endl;
        //�ж��û���������
        LoginResult* ret = new LoginResult();
        send(c_sock, (const char*)ret, sizeof(LoginResult), 0);
    }
    break;
    case CMD_LOGOUT:
    {
        recv(c_sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//DataHeader�ѱ����ܣ���Ҫƫ��DataHeader����������ʣ������
        Logout* logout = reinterpret_cast<Logout*>(szRecv);
        std::cout << "receive:CMD_LOGOUT from client socket:" << c_sock << "\tdata length:" << logout->dataLength <<
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
