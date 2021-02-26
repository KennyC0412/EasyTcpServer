#pragma once

#ifndef _CLIENT_H_
#define _CLIENT_H_
#include "messageHeader.h"
#include "pre.h"
#include "CELLClient.h"
#include "CELLBuffer.h"
#include "FDset.hpp"

class TcpClient {
public:
	TcpClient():isConnect(false)
	{}
	virtual ~TcpClient() { 
			Close();
	}
	//��ʼ��socket
	int initSocket();
	//���ӷ�����
	int connServer(const char* , short);
	//�ر�socket
	void Close();
	//��������
	int readData();
	//��������
	int writeData(DataHeaderPtr& );
	//������������
	bool onRun(int);
	bool isRun();
	virtual void onNetMsg(DataHeader*) = 0;
	CELLClientPtr _client = nullptr;

public:
	unsigned int nRecvMsgID = 1;
	unsigned int nSendMsgID = 1;
	bool _bCheckMsgID = false;
private:
	bool isConnect;
	FDset fdRead;
	FDset fdWrite;
};


#endif