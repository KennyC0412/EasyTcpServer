#ifdef  _WIN32
#pragma once
#endif

#ifndef  _NETENV_MANAGE_H_
#define _NETENV_MANAGE_H_
#include "pre.h"

class NetEnv
{
public:
	NetEnv();
	~NetEnv();
	static void init();
};
#endif // ! _NET_ENV_START_
