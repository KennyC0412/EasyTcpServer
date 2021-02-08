
#include "pre.h"
#include "client.h"

void cmdThread();
void sendThread(int id);
extern bool g_bRun;

int main()
{
	std::thread t(cmdThread);
	t.detach();

	for (int i = 0; i < tCount; ++i) {
		std::thread t1(sendThread,i+1 );
		t1.detach();
	}
	while (g_bRun) {
		Sleep(100);
	}
	getchar();
	return 0;
}