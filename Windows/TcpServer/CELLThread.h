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
	//启动线程
	void Start(EventCall, EventCall, EventCall);
	//关闭线程
	void Close();
	//返回运行状态
	 bool Status() { return isRun; }
	 //异常退出
	 void Exit();
	 //休眠
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
	//运行标志位
	bool isRun = false;
	Semaphore sem;
};

#endif