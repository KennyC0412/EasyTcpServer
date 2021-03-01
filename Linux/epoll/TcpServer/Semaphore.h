#ifdef _WIN32
#pragma once
#endif

#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <condition_variable>

class Semaphore {
public:
	Semaphore():waitTime(0),wakeTime(0){}
	~Semaphore(){}
	void wait();
	void wakeup();
private:
	std::mutex m;
	std::condition_variable cv;
	//等待计数
	int waitTime;
	//唤醒计数
	int wakeTime;
};
#endif
