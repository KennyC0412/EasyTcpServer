#include "CELLTask.h"


void CellTaskServer::addTask(CellTask *task)
{
	std::lock_guard<std::mutex> lk(mute);
	taskBuf.push_back(task);
}

void CellTaskServer::Start()
{
	std::thread t(std::mem_fn(&CellTaskServer::onRun), this);
	t.detach();
}

void CellTaskServer::onRun()
{
	while (true) {
		if (!taskBuf.empty()) {
			std::lock_guard<std::mutex> lk(mute);
			for (auto t : taskBuf) {
				taskList.push_back(t);
			}
			taskBuf.clear();
		}
		//������ʱ�ȴ�
		if (taskList.empty()) {
			std::chrono::milliseconds t(5);
			std::this_thread::sleep_for(t);
			continue;
		}
		//��������
		for (auto t : taskList) {
			t->doTask();
			delete t;
		}
		//������������
		taskList.clear();
	}
	
}

void sendMsg2Client::doTask()
{
	pClient->sendData(pHeader);
	delete pHeader;
}
