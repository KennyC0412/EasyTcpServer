#include "Semaphore.h"
#include <chrono>
#include <thread>

void Semaphore::wait()
{
	std::unique_lock<std::mutex> lock(m);
	if (--waitTime < 0) {
		//该线程阻塞 直到其他地方有notify唤醒它
		cv.wait(lock, [&]()->bool { return wakeTime > 0; });
		--wakeTime;
	}
}

void Semaphore::wakeup()
{
	std::unique_lock<std::mutex> lock(m);
	if (++waitTime <= 0) {
		++wakeTime;
		cv.notify_one();
	}
}
