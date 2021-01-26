#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
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
	WORD ver = MAKEWORD(1, 2);
	WSADATA dat;
	WSAStartup(ver, &dat);
	//简易TCP客户端
	//新建一个socket
	SOCKET c_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (c_sock == INVALID_SOCKET) {
		std::cerr << "failed to create socket." << std::endl;
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
			Login login = { "KennyC0412","123456" };
			DataHeader dh = { sizeof(login),CMD_LOGIN };
			//发送命令
			send(c_sock, (const char*)&dh, sizeof(DataHeader), 0);
			send(c_sock, (const char*)&login, sizeof(Login), 0);
			//接收服务器返回数据
			DataHeader rdh{};
			LoginResult loginRet{};
			recv(c_sock, (char*)&rdh, sizeof(DataHeader), 0);
			recv(c_sock, (char*)&loginRet, sizeof(LoginResult), 0);
			std::cout << "Login result: " << loginRet.result << std::endl;
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			Logout logout{ "KennyC0412" };
			DataHeader dh = { sizeof(Logout),CMD_LOGOUT };
			//发送命令
			send(c_sock, (const char*)&dh, sizeof(DataHeader), 0);
			send(c_sock, (const char*)&logout, sizeof(logout), 0);
			//接收服务器返回数据
			DataHeader rdh{};
			LogoutResult logoutRet{};
			recv(c_sock, (char*)&rdh, sizeof(DataHeader), 0);
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