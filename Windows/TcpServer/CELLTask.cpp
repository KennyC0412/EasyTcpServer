#include "CELLTask.h"
#include "CELLLog.h"

//void CELLTaskServer::addTask(CELLTaskPtr& task)
//{
//	std::lock_guard<std::mutex> lk(_mutex);
//	_taskBuf.push_back(task);
//}

void CELLTaskServer::addTask(std::function<void()> task)
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
	CELLLog_Info("Task Server Closed.");
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
		//做任务
		for (auto t : _taskList) {
			t();
		}
		//清空已完成任务
		_taskList.clear();
	}
	//将结束时还在缓冲队列的任务完成
	for (auto t : _taskBuf) {
		t();
	}
}

void sendMsg2Client::doTask()
{
	pClient->push(pHeader);	//发送缓冲区已满 消息发送失败
	pClient->sendData();
}
