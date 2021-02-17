#include "CELLTask.h"


void CellTaskServer::addTask(CellTaskPtr& task)
{
	std::lock_guard<std::mutex> lk(mute);
	taskBuf.push_back(task);
}

void CellTaskServer::Start()
{
	isRun = true;
	std::thread t(std::mem_fn(&CellTaskServer::onRun), this);
	t.detach();
}

void CellTaskServer::Close()
{		
	std::cout << "Task Server Closed." << std::endl;
	if (isRun) {
		isRun = false;
		sem.wait();
	}
}

void CellTaskServer::onRun()
{
	while (isRun) {
		if (!taskBuf.empty()) {
			std::lock_guard<std::mutex> lk(mute);
			for (auto t : taskBuf) {
				taskList.push_back(t);
			}
			taskBuf.clear();
		}
		//无任务时等待
		if (taskList.empty()) {
			std::chrono::milliseconds t(5);
			std::this_thread::sleep_for(t);
			continue;
		}
		for (auto t : taskList) {
			t->doTask();
		}
		//清空已完成任务
		taskList.clear();
	}
	sem.wakeup();
}

void sendMsg2Client::doTask()
{
	pClient->sendData(pHeader);
}
