#ifdef  _WIN32
#pragma once
#endif

#ifndef  _NETENV_MANAGE_H_
#define _NETENV_MANAGE_H_
#include "pre.h"

class NetEnv
{
public:
	NetEnv() {
#ifdef _WIN32
		WORD ver = MAKEWORD(2, 0);
		WSADATA dat;
		WSAStartup(ver, &dat);
#endif // _WIN32
	}
	~NetEnv()
	{
#ifdef _WIN32
		WSACleanup();
#endif // _WIN32
	}
	static void init()
	{
		static NetEnv obj;
	}
};
#endif // ! _NET_ENV_START_
