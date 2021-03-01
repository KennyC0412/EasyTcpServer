#ifndef _CELL_TASK_H_
#define _CELL_TASK_H_

#include <thread>
#include <mutex>
#include <list>
#include <functional>
#include "CELLClient.h"
#include "CELLThread.h"

using CELLClientPtr = std::shared_ptr<CELLClient>;
class CELLTask;
using CELLTaskPtr = std::shared_ptr<CELLTask>;

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
	//void addTask(CELLTaskPtr &);
	void addTask(std::function<void()>);
	//启动工作线程
	void Start();
	//关闭工作线程
	void Close();

protected:
	//工作函数
	void onRun(CELLThread *);
private:
	//任务表
	std::list<std::function<void()>> _taskList;
	//任务缓冲区
	std::list<std::function<void()>> _taskBuf;
	//修改缓冲区时加锁
	std::mutex _mutex;
	CELLThread _thread;
};

#endif