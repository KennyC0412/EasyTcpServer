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
    WORD ver = MAKEWORD(1, 3); //�汾��
    WSADATA dat;
    WSAStartup(ver, &dat);
    //�½�һ��socket
    SOCKET s_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s_sock == INVALID_SOCKET) {
        std::cerr << "failed to create socket." << std::endl;
    }
    //�󶨽������ӵ�����˿�
    sockaddr_in _sin;
    memset(&_sin, 0, sizeof(_sin));
    _sin.sin_port = htons(PORT);
    _sin.sin_family = AF_INET;
    _sin.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    if (bind(s_sock, (sockaddr*)&_sin, sizeof(_sin)) == SOCKET_ERROR)
        std::cerr << "failed to bind." << std::endl;
    else
        std::cout << "successed to bind." << std::endl;
    //listen��������˿�
    if (listen(s_sock, BACKLOG) == SOCKET_ERROR)
        std::cerr << "failed to listen." << std::endl;
    else
        std::cout << "successed to listen." << std::endl;
   
    //��ͻ��˷��ͼ���������
    while (true) {
        //������socket
        fd_set fdRead;
        fd_set fdWrite;
        fd_set fdExp;
        //������
        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExp);
        //�����������뼯��
        FD_SET(s_sock, &fdRead);
        FD_SET(s_sock, &fdWrite);
        FD_SET(s_sock, &fdExp);

        //��ÿ��ѭ���н��ͻ��˼����������
        for (size_t i = 0; i < g_clients.size(); ++i) {
            FD_SET(g_clients[i], &fdRead);
        }

        //ʹ��select�����������м���
        timeval t{ 1,0 };
       int ret =  select(s_sock + 1, &fdRead, &fdWrite, &fdExp, &t);
       //û���Ѽ�������������select�������
       if (ret < 0) {
           std::cout << "select finished." << std::endl;
           break;
       }
       //�ж��������Ƿ��ڼ����У���Ϊselect������δ��Ӧ������
       if (FD_ISSET(s_sock, &fdRead) ){
           FD_CLR(s_sock, &fdRead);
           //���ܿͻ�������
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
               //�洢����ͻ��˵��׽���
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

    //�������رգ��ر���������
    std::for_each(g_clients.begin(), g_clients.end(), [](SOCKET s) { closesocket(s); });
    closesocket(s_sock);
    std::cout << "Connect closed." << std::endl;
    WSACleanup();
    return 0;
}
