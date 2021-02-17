#include "Semaphore.h"
#include <chrono>
#include <thread>

void Semaphore::wait()
{
	std::unique_lock<std::mutex> lock(m);
	if (--waitTime < 0) {
		//���߳����� ֱ�������ط���notify������
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
