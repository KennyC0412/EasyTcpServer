#ifdef _WIN32
#pragma once
#endif

#ifndef _CELL_THREAD_H_
#define _CELL_THREAD_H_

#include "Semaphore.h"
#include <functional>

class CELLThread
{
private:
	using EventCall = std::function<void(CELLThread*)>;
public:
	//�����߳�
	void Start(EventCall, EventCall, EventCall);
	//�ر��߳�
	void Close();
	//��������״̬
	 bool Status() { return isRun; }
	 //�쳣�˳�
	 void Exit();
	 //����
	 void Sleep(time_t dt)
	 {
		 std::chrono::milliseconds time(dt);
		 std::this_thread::sleep_for(time);
	 }
protected:
	void onWork();
private:
	EventCall onCreate;
	EventCall onRun;
	EventCall onDestroy;
	std::mutex _mutex;
	//���б�־λ
	bool isRun = false;
	Semaphore sem;
};

#endif