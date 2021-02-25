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
	//�ȴ�����
	int waitTime;
	//���Ѽ���
	int wakeTime;
};
#endif
