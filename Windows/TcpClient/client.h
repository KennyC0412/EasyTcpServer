#pragma once

#ifndef _CLIENT_H_
#define _CLIENT_H_
#include "messageHeader.h"
#include "pre.h"
#include "CELLClient.h"
#include "CELLBuffer.h"

class TcpClient {
public:
	TcpClient():isConnect(false)
	{
	}
	virtual ~TcpClient() { 
			Close();
	}
	//��ʼ��socket
	int initSocket();
	//���ӷ�����
	int connServer(const char* , short = PORT );
	//�ر�socket
	void Close();
	//��������
	int readData();
	//��������
	int writeData(DataHeaderPtr& );
	//������������
	void onRun();
	bool isRun();
	virtual void onNetMsg(DataHeader*) = 0;
protected:
	CELLClientPtr _client = nullptr;
	bool isConnect;
};


#endif