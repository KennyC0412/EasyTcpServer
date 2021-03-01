#ifndef _CELL_SELECT_H_
#define _CELL_SELECT_H_

#include "CellServer.h"

class CELLSelect:public CELLServer
{
public:
	CELLSelect(SOCKET s):CELLServer(s){}
	bool core();
	void readData();
	void writeData();

private:
	FDset _fdRead_back;
	FDset _fdRead;
	FDset _fdWrite;
	SOCKET _maxSock;
};

#endif