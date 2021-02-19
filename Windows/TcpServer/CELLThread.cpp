#include "CELLThread.h"

void CELLThread::Start(EventCall create = nullptr, EventCall run = nullptr, EventCall close = nullptr)
{
	std::lock_guard<std::mutex> lk(_mutex);
	if (create)
		onCreate = create;
	if (run)
		onRun = run;
	if (close)
		onDestroy = close;
	isRun = true;
	std::thread t(std::mem_fn(&CELLThread::onWork), this);
	t.detach();
}

void CELLThread::Close()
{
	std::lock_guard<std::mutex> lk(_mutex);
	if (isRun) {
		isRun = false;
		sem.wait();
	}
}

void CELLThread::Exit()
{
	std::lock_guard<std::mutex> lk(_mutex);
	if (isRun) {
		isRun = false;
	}
}

void CELLThread::onWork()
{
	if (onCreate) {
		onCreate(this);
	}
	if (onRun) {
		onRun(this);
	}
	if (onDestroy) {
		onDestroy(this);
	}
	sem.wakeup();
}
