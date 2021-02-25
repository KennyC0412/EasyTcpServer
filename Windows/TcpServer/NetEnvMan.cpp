#include "NetEnvMan.h"
#include "pre.h"

NetEnv::NetEnv()
{
#ifdef _WIN32
		WORD ver = MAKEWORD(2, 0);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif // _WIN32

}

NetEnv::~NetEnv()
{
#ifdef _WIN32
	WSACleanup();
#endif // _WIN32
}

void NetEnv::init()
{
	static NetEnv obj;
}
