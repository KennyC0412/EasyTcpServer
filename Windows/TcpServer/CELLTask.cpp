#include "CELLTask.h"


void CellTaskServer::addTask(CellTaskPtr& task)
{
	std::lock_guard<std::mutex> lk(_mutex);
	_taskBuf.push_back(task);
}

void CellTaskServer::Start()
{
	_thread.Start(nullptr, [this](CellThread* pThread) { onRun(pThread); },nullptr);
}

void CellTaskServer::Close()
{		
	_thread.Close();
}

void CellTaskServer::onRun(CellThread * pThread)
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
	pClient->sendData(pHeader);
}
