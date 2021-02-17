#include "Semaphore.h"
#include <chrono>
#include <thread>

void Semaphore::wait()
{
	waitExit = true;
	while (waitExit) {
		std::chrono::milliseconds t(1);
		std::this_thread::sleep_for(t);
	}
}

void Semaphore::wakeup()
{
	waitExit = false;
}
