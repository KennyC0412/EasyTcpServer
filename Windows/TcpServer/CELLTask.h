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

//任务类型-基类
class CellTask 
{
public:
	CellTask(){
	}
	virtual ~CellTask(){
	}
	//执行任务
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
	//添加任务
	void addTask(CellTaskPtr &);
	//启动工作线程
	void Start();
	//工作函数
	void onRun();
private:
	//任务表
	std::list<CellTaskPtr> taskList;
	//任务缓冲区
	std::list<CellTaskPtr> taskBuf;
	//修改缓冲区时加锁
	std::mutex mute;
};

class sendMsg2Client :public CellTask
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