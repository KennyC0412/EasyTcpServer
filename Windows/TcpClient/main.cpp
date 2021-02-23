#include "CELLtimestamp.hpp"
#include "pre.h"
#include "client.h"
#include "CELLLog.h"

void cmdThread();
void sendThread(int id);
extern bool g_bRun;
extern std::atomic_int readyCount;
extern std::atomic_int sendCount;

int main()
{
	CELLLog::getInstance().setLogPath("temp\\clientLog.txt", 'w');
	std::thread t(cmdThread);
	t.detach();

	for (int i = 0; i < tCount; ++i) {
		std::thread t1(sendThread, i + 1);
		t1.detach();
	}

	CELLTimestamp tTime;
	while (g_bRun) {
		auto t = tTime.getElapsedSecond();
		int num = 0;
		if (t >= 1.0) {
			num = sendCount;
			CELLLog::Info("thread:<", tCount, ">, ", "client: <", cCount, ">,send count : < ", num, ">");
			sendCount = 0;
			tTime.update();
		}
	}
	return 0;
}

