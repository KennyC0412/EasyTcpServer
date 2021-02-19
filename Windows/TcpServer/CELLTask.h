#pragma once
#ifndef _CELL_TASK_H_
#define _CELL_TASK_H_

#include <thread>
#include <mutex>
#include <list>
#include <functional>
#include "clientsocket.hpp"
#include "CELLThread.h"

using ClientSocketPtr = std::shared_ptr<ClientSocket>;
class CELLTask;
using CELLTaskPtr = std::shared_ptr<CELLTask>;
using DataHeaderPtr = std::shared_ptr<DataHeader>;

//��������-����
class CELLTask 
{
public:
	CELLTask(){
	}
	virtual ~CELLTask(){
	}
	//ִ������
	virtual void doTask() {
	}
private:
	
};

class CELLTaskServer
{
public:
	CELLTaskServer()
	{

	}
	~CELLTaskServer()
	{
	}
	//�������
	void addTask(CELLTaskPtr &);
	//���������߳�
	void Start();
	//�رչ����߳�
	void Close();

protected:
	//��������
	void onRun(CELLThread *);
private:
	//�����
	std::list<CELLTaskPtr> _taskList;
	//���񻺳���
	std::list<CELLTaskPtr> _taskBuf;
	//�޸Ļ�����ʱ����
	std::mutex _mutex;
	CELLThread _thread;
};

class sendMsg2Client :public CELLTask
{
public:
	sendMsg2Client(ClientSocketPtr& client, DataHeaderPtr& dh) :pClient(client), pHeader(dh) {

	}
	//ִ������
	virtual void doTask();
private:
	ClientSocketPtr pClient;
	DataHeaderPtr pHeader;
};
#endif