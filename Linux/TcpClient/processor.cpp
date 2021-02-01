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

int processor(SOCKET c_sock) {
    char szRecv[1024] = {};
    DataHeader* header = reinterpret_cast<DataHeader*>(szRecv);
    if (recv(c_sock, szRecv, sizeof(DataHeader), 0) <= 0) {
        std::cout << "lost connection to server." << c_sock << std::endl;
        return -1;
    }
    switch (header->cmd) {
    case CMD_LOGIN_RESULT:
    {
        recv(c_sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//DataHeader已被接受，需要偏移DataHeader长度来接受剩余数据
        LoginResult* loginResult = reinterpret_cast<LoginResult*>(szRecv);
        std::cout << "receive:CMD_LOGIN_RESULT from server. result:" << loginResult->result << "\tdata length:" << loginResult->dataLength << std::endl;
    }
    break;
    case CMD_LOGOUT_RESULT:
    {
        recv(c_sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//DataHeader已被接受，需要偏移DataHeader长度来接受剩余数据
        LogoutResult* logoutResult = reinterpret_cast<LogoutResult *>(szRecv);
        std::cout << "receive:CMD_LOGOUT_RESULT from server. result:" << logoutResult->result <<"\tdata length:" << logoutResult->dataLength << std::endl;
    }
    break;
    case CMD_NEW_USER_JOIN:
    {
        recv(c_sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader), 0);//DataHeader已被接受，需要偏移DataHeader长度来接受剩余数据
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
