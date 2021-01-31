#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
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
struct Login:public DataHeader
{
	Login() {
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32];
	char passWord[32];
};

struct LoginResult:public DataHeader
{
	LoginResult() {
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};

struct Logout:public DataHeader
{
	Logout() {
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32];
};

struct LogoutResult:public DataHeader
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
	WORD ver = MAKEWORD(1, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//简易TCP客户端
	//新建一个socket
	SOCKET c_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (c_sock == INVALID_SOCKET) {
		std::cerr << "failed to create socket." << std::endl;
		exit(0);
	}
	//连接服务器
	sockaddr_in _sin;
	memset(&_sin, 0, sizeof(_sin));
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(PORT);
	_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (connect(c_sock, (sockaddr*)&_sin, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		std::cerr << "failed to connect." << std::endl;
	}
	else {
		std::cout << "successed to connect." << std::endl;
		char buffer[1024] = { };
		if(0 < recv(c_sock, buffer, 1024, 0))
			std::cout << buffer;
	}

	while (true) {
		//输入命令请求
		char cmdBuf[128] = {};
		std::cin >> cmdBuf;
		if (0 == strcmp(cmdBuf, "exit")) {
			std::cout << "receive exit command, program quit." << std::endl;
			break;
		}
		else if (0 == strcmp(cmdBuf, "login")) {
			Login login;
			strcpy(login.userName, "Kenny");
			strcpy(login.passWord, "123456");
			//发送命令
			send(c_sock, (const char*)&login, sizeof(login), 0);
			//接收服务器返回数据
			LoginResult loginRet{};
			recv(c_sock, (char*)&loginRet, sizeof(LoginResult), 0);
			std::cout << "Login result: " << loginRet.result << std::endl;
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			Logout logout;
			strcpy(logout.userName, "Kenny");
			//发送命令
			send(c_sock, (const char*)&logout, sizeof(logout), 0);
			//接收服务器返回数据
			LogoutResult logoutRet{};
			recv(c_sock, (char*)&logoutRet, sizeof(LogoutResult), 0);
			std::cout << "Logout result: " << logoutRet.result << std::endl;
		}
		else {
			std::cout << "unknown command,please input again: ";
		}
	}

	//关闭closesocket
	closesocket(c_sock);
	std::cout << "Connect closed." << std::endl;
	WSACleanup();
	Sleep(1000);
	return 0;
}