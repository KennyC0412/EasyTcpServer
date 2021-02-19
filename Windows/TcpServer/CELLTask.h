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

//任务类型-基类
class CELLTask 
{
public:
	CELLTask(){
	}
	virtual ~CELLTask(){
	}
	//执行任务
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
	//添加任务
	void addTask(CELLTaskPtr &);
	//启动工作线程
	void Start();
	//关闭工作线程
	void Close();

protected:
	//工作函数
	void onRun(CELLThread *);
private:
	//任务表
	std::list<CELLTaskPtr> _taskList;
	//任务缓冲区
	std::list<CELLTaskPtr> _taskBuf;
	//修改缓冲区时加锁
	std::mutex _mutex;
	CELLThread _thread;
};

class sendMsg2Client :public CELLTask
{
public:
	sendMsg2Client(ClientSocketPtr& client, DataHeaderPtr& dh) :pClient(client), pHeader(dh) {

	}
	//执行任务
	virtual void doTask();
private:
	ClientSocketPtr pClient;
	DataHeaderPtr pHeader;
};
#endif