#ifndef _SELECT_SERVER_H_
#define _SELECT_SERVER_H_

#include "server.h"

class SelectServer:public TcpServer 
{
public:
	SelectServer(int maxClient):TcpServer(maxClient) {}
	void onRun(CELLThread* pThread);
	void Start(int nThread);
private:
};

#endif