#ifdef _WIN32
#pragma once
#endif

#ifndef _CELL_CLIENT_H_
#define _CELL_CLIENT_H_

#include "pre.h"
#include "messageHeader.h"
#include "ObjectPool.hpp"
#include "CELLBuffer.h"

//�������ʱ��
#define CLIENT_HEART_DEAD_TIME 60000
//�������ڻ�����Ϣ���͸��ͻ��˵�ʱ��
#define CLIENT_SEND_BUFF_TIME 200


class CELLClient:public ObjectPoolBase<CELLClient,10000>
{
public:
	CELLClient(SOCKET sock) :_sockfd(sock){
		
		rstDtHeart();
		rstDtSend();
	}
	~CELLClient() { closesocket(_sockfd); }
	SOCKET getSock() { return _sockfd; }
	//����д�뻺�����ȴ��ɷ���
	int push(DataHeaderPtr&);
	//�Ƴ�ͷ����Ϣ
	void pop_front_Msg();
	//���ͻ���������
	int sendData();
	//��������
	int recvData();
	//��������ʱ��
	void rstDtHeart() { dtHeart = 0; }
	//���÷���ʱ��
	void rstDtSend() { dtSend = 0; }
	//���������
	bool checkHeart(time_t);
	//��ʱ���ͼ��
	void checkSend(time_t);
	//����Ƿ�����Ϣ
	bool hasMsg();
	//�õ�������ͷ������
	DataHeader* front_Msg();
private:
	//���ͻ�����
	CELLBuffer _sendBuff = CELLBuffer(SEND_BUFF_SIZE);
	//���ջ�����
	CELLBuffer _recvBuff = CELLBuffer(RECV_BUFF_SIZE);
	SOCKET _sockfd;
	time_t dtHeart;
	time_t dtSend;

};
using CELLClientPtr = std::shared_ptr<CELLClient>;

#endif // !_CELL_CLIENT_H_
