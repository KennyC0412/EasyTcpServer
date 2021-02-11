#pragma once
#ifndef _CELL_TASK_H_
#define _CELL_TASK_H_

#include <thread>
#include <mutex>
#include <list>
#include <functional>
#include "clientsocket.hpp"

//��������-����
class CellTask 
{
public:
	CellTask(){
	}
	virtual ~CellTask(){
	}
	//ִ������
	virtual void doTask() {
	}
private:
	
};

class CellTaskServer
{
public:
	CellTaskServer()
	{

	}
	~CellTaskServer()
	{

	}
	//�������
	void addTask(CellTask *);
	//���������߳�
	void Start();
	//��������
	void onRun();
private:
	//�����
	std::list<CellTask*> taskList;
	//���񻺳���
	std::list<CellTask*> taskBuf;
	//�޸Ļ�����ʱ����
	std::mutex mute;
};

class sendMsg2Client :public CellTask
{
public:
	sendMsg2Client(ClientSocket* client, DataHeader* dh) :pClient(client), pHeader(dh) {

	}
	//ִ������
	virtual void doTask();
private:
	ClientSocket* pClient;
	DataHeader* pHeader;
};
#endif