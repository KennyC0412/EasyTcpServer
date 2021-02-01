#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock2.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include "pre.h"

int processor(SOCKET c_sock) {
    //������
    char szRecv[1024] = {};
    //������յ�������
    DataHeader* header = reinterpret_cast<DataHeader*>(szRecv);
    if (recv(c_sock, szRecv, sizeof(DataHeader), 0) <= 0) {
        std::cout << "lost connection to server." << c_sock << std::endl;
        return -1;
    }
    switch (header->cmd) {
    case CMD_LOGIN_RESULT:
    {
        recv(c_sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//DataHeader�ѱ����ܣ���Ҫƫ��DataHeader����������ʣ������
        LoginResult* loginResult = reinterpret_cast<LoginResult*>(szRecv);
        std::cout << "receive:CMD_LOGIN_RESULT from server. result:" << loginResult->result << "\tdata length:" << loginResult->dataLength << std::endl;
    }
    break;
    case CMD_LOGOUT_RESULT:
    {
        recv(c_sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//DataHeader�ѱ����ܣ���Ҫƫ��DataHeader����������ʣ������
        LogoutResult* logoutResult = reinterpret_cast<LogoutResult *>(szRecv);
        std::cout << "receive:CMD_LOGOUT_RESULT from server. result:" << logoutResult->result <<"\tdata length:" << logoutResult->dataLength << std::endl;
    }
    break;
    case CMD_NEW_USER_JOIN:
    {
        recv(c_sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//DataHeader�ѱ����ܣ���Ҫƫ��DataHeader����������ʣ������
        NewUserJoin * newUser = reinterpret_cast<NewUserJoin*>(szRecv);
        std::cout << "receive:CMD_NEW_USER_JOIN from server. socket:" << newUser->sock << "\tdata length:" << newUser->dataLength << std::endl;
    }
    break;
    default:
        DataHeader header = { 0,CMD_ERROR };
        send(c_sock, (const char*)&header, sizeof(DataHeader), 0);
    }
    return 0;
}
