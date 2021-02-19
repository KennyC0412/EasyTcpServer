#include "CELLTask.h"


void CELLTaskServer::addTask(CELLTaskPtr& task)
{
	std::lock_guard<std::mutex> lk(_mutex);
	_taskBuf.push_back(task);
}

void CELLTaskServer::Start()
{
	_thread.Start(nullptr, [this](CELLThread* pThread) { onRun(pThread); },nullptr);
}

void CELLTaskServer::Close()
{		
	_thread.Close();
}

void CELLTaskServer::onRun(CELLThread * pThread)
{
	while (pThread->Status()) {
		if (!_taskBuf.empty()) {
			std::lock_guard<std::mutex> lk(_mutex);
			for (auto t : _taskBuf) {
				_taskList.push_back(t);
			}
			_taskBuf.clear();
		}
		//无任务时等待
		if (_taskList.empty()) {
			std::chrono::milliseconds t(5);
			std::this_thread::sleep_for(t);
			continue;
		}
		for (auto t : _taskList) {
			t->doTask();
		}
		//清空已完成任务
		_taskList.clear();
	}
	std::cout << "Task Server Closed." << std::endl;
}

void sendMsg2Client::doTask()
{
	pClient->writeData(pHeader);	//发送缓冲区已满 消息发送失败
	pClient->sendData();
}
