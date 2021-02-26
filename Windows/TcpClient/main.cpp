#include "CELLtimestamp.hpp"
#include "pre.h"
#include "MyClient.hpp"
#include "CELLLog.h"
#include "CELLConfig.hpp"

void WorkThread(CELLThread* ,int id);
extern bool g_bRun;
extern std::atomic_int sendCount;

 const char* strIP;
 short nPort;
 int nThread;
int nClient;
int nMsg;
int sleepTime;
std::vector<MyClient*> client;

int main(int argc,char* argv[])
{
	CELLConfig::getInstance().Init(argc, argv);

	strIP = CELLConfig::getInstance().getCMD("strIP", "127.0.0.1");
	nPort = CELLConfig::getInstance().getCMD("nPort", 8888);
	nThread = CELLConfig::getInstance().getCMD("nThread", 4);
	nClient = CELLConfig::getInstance().getCMD("nClient",2000 );
	nMsg = CELLConfig::getInstance().getCMD("nMsg", 100);
	sleepTime = CELLConfig::getInstance().getCMD("sleepTime", 1000);
	client.resize(nClient);
	CELLLog::getInstance().setLogPath("temp\\clientLog.txt", 'w', false);

	CELLThread tCMD;
	tCMD.Start(nullptr, [](CELLThread* p) {
		while (p->Status()) {
			char cmdBuf[256];
			std::cin >> cmdBuf;
			if (0 == strcmp(cmdBuf, "exit")) {
				p->Exit();
				std::cout << "cmd thread quit." << std::endl;
				break;
			}
			else {
				std::cout << "Wrong command.please input again:";
			}
		}
		},nullptr);

	std::vector<CELLThread*> threads;
	for (int i = 0; i < nThread; ++i) {
		CELLThread* _t = new CELLThread();
		_t->Start(nullptr, [i](CELLThread *pThread) {
			WorkThread(pThread, i + 1);
			},nullptr);
		threads.push_back(_t);
	}

	CELLTimestamp tTime;
	while (tCMD.Status()) {
		auto t = tTime.getElapsedSecond();
		if (t >= 1.0) {
			CELLLog::Info("thread:<", nThread, ">, ", "client: <", nClient, ">,send count : < ", (int)sendCount, ">");
			sendCount = 0;
			tTime.update();
		}
	}
	return 0;
}

