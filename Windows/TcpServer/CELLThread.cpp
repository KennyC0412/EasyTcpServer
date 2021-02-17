#include "CELLThread.h"

void CellThread::Start(EventCall create = nullptr, EventCall run = nullptr, EventCall close = nullptr)
{
	std::lock_guard<std::mutex> lk(_mutex);
	if (create)
		onCreate = create;
	if (run)
		onRun = run;
	if (close)
		onDestroy = close;
	isRun = true;
	std::thread t(std::mem_fn(&CellThread::onWork), this);
	t.detach();
}

void CellThread::Close()
{
	std::lock_guard<std::mutex> lk(_mutex);
	if (isRun) {
		isRun = false;
		sem.wait();
	}
}

void CellThread::Exit()
{
	std::lock_guard<std::mutex> lk(_mutex);
	if (isRun) {
		isRun = false;
	}
}

void CellThread::onWork()
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
