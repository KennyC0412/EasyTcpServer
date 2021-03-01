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
	//void addTask(CELLTaskPtr &);
	void addTask(std::function<void()>);
	//���������߳�
	void Start();
	//�رչ����߳�
	void Close();

protected:
	//��������
	void onRun(CELLThread *);
private:
	//�����
	std::list<std::function<void()>> _taskList;
	//���񻺳���
	std::list<std::function<void()>> _taskBuf;
	//�޸Ļ�����ʱ����
	std::mutex _mutex;
	CELLThread _thread;
};

#endif