#pragma once
#ifndef _CELL_TASK_H_
#define _CELL_TASK_H_

#include <thread>
#include <mutex>
#include <list>
#include <functional>
#include "clientsocket.hpp"

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
public:
	CellTaskServer()
	{

	}
	~CellTaskServer()
	{

	}
	//添加任务
	void addTask(CellTask *);
	//启动工作线程
	void Start();
	//工作函数
	void onRun();
private:
	//任务表
	std::list<CellTask*> taskList;
	//任务缓冲区
	std::list<CellTask*> taskBuf;
	//修改缓冲区时加锁
	std::mutex mute;
};

class sendMsg2Client :public CellTask
{
public:
	sendMsg2Client(ClientSocket* client, DataHeader* dh) :pClient(client), pHeader(dh) {

	}
	//执行任务
	virtual void doTask();
private:
	ClientSocket* pClient;
	DataHeader* pHeader;
};
#endif