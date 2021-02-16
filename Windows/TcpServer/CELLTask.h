#pragma once
#ifndef _CELL_TASK_H_
#define _CELL_TASK_H_

#include <thread>
#include <mutex>
#include <list>
#include <functional>
#include "clientsocket.hpp"

using ClientSocketPtr = std::shared_ptr<ClientSocket>;
class CellTask;
using CellTaskPtr = std::shared_ptr<CellTask>;
using DataHeaderPtr = std::shared_ptr<DataHeader>;

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
	using CellTask = std::function<void()>;
public:
	CellTaskServer()
	{

	}
	~CellTaskServer()
	{

	}
	//�������
	void addTask(CellTaskPtr &);
	//���������߳�
	void Start();
	//��������
	void onRun();
private:
	//�����
	std::list<CellTaskPtr> taskList;
	//���񻺳���
	std::list<CellTaskPtr> taskBuf;
	//�޸Ļ�����ʱ����
	std::mutex mute;
};

class sendMsg2Client :public CellTask
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