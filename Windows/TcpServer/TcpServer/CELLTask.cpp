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
		//������ʱ�ȴ�
		if (_taskList.empty()) {
			std::chrono::milliseconds t(5);
			std::this_thread::sleep_for(t);
			continue;
		}
		//������
		for (auto t : _taskList) {
			t();
		}
		//������������
		_taskList.clear();
	}
	//������ʱ���ڻ�����е��������
	for (auto t : _taskBuf) {
		t();
	}
}

void sendMsg2Client::doTask()
{
	pClient->push(pHeader);	//���ͻ��������� ��Ϣ����ʧ��
	pClient->sendData();
}
