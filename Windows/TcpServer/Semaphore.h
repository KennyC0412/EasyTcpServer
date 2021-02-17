#ifdef _WIN32
#pragma once
#endif

#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_


class Semaphore {
public:
	Semaphore(){}
	~Semaphore(){}
	void wait();
	void wakeup();
private:
	bool waitExit = false;
};
#endif
