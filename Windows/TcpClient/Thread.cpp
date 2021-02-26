#define WIN32_LEAN_AND_MEAN
#include "pre.h"
#include "messageHeader.h"
#include "client.h"
#include "CELLtimestamp.hpp"
#include "MyClient.hpp"
#include "CELLLog.h"
#include "CELLThread.h"

extern const char* strIP;
extern short nPort;
extern  int nClient;
extern int nThread;
extern int nMsg;
extern int sleepTime;

bool g_bRun = true;
std::atomic_int readyCount = 0;
std::atomic_int sendCount{ 0 };
extern std::vector<MyClient*> client;




void WorkThread(CELLThread* pThread,int id)
{
	int c = nClient / nThread;
	int begin = (id - 1) * c;
	int end = id * c;
	for (int i = begin; i < end; ++i) {
		if (!pThread->Status()) {
			break;
		}
		client[i] = new MyClient();
		CELLThread::Sleep(0);
	}

	for (int i = begin; i < end; ++i) {
		if (!pThread->Status()) {
			break;
		}
		if (INVALID_SOCKET == client[i]->initSocket()) {
			break;
		}
		if (SOCKET_ERROR == client[i]->connServer(strIP, nPort)) {
			break;
		}
		CELLThread::Sleep(0);
	}
	CELLLog_Info("thread<", id, ">,Connect = <begin:", begin, ",end:", end, ">");

	readyCount++;
	while (readyCount < nThread) 
	{//等待所有连接完成
		std::chrono::milliseconds t(10);
		std::this_thread::sleep_for(t);
	}

	CELLSendStream s;
	s.setNetCMD(CMD_LOGOUT);
	s.writeInt8(1);
	s.writeInt16(2);
	s.writeInt32(3);
	s.writeFloat(4.5f);
	s.writeDouble(6.7);
	char str[] = "client";
	s.writeString(str);
	std::string a{ "abababa" };
	s.writeString(a);
	int b[] = { 1,2,3,4,5 };
	s.writeArray(b, sizeof(b)/sizeof(b[0]));
	//写入数据长度
	s.finish();
	DataHeader* data = reinterpret_cast<DataHeader*>(s.data());
	DataHeaderPtr pdata = std::shared_ptr<DataHeader>(data);

	DataHeaderPtr xdata = std::make_shared<Login>();

	while (g_bRun) {
		time_t old = CELLTime::getNowInMilliSec();
	
			//发送消息限制
			for (int j = 0; j < nMsg; ++j) {
				for (int i = begin; i < end; ++i) {
					{
						if (client[i]->isRun()) {
							if (SOCKET_ERROR != client[i]->writeData(xdata)) {
								++sendCount;
							}
						}
					}
				}
			}
		
		for (int i = begin; i < end; ++i) {
			if (client[i]->isRun()) {
				if (!client[i]->onRun(0)) {
					nClient--;
					continue;
				}
			}
		}
		time_t now = CELLTime::getNowInMilliSec();
		while (now - old < 1000) {
		 now = CELLTime::getNowInMilliSec();
		}
	
}

	for (int i = begin; i < end; ++i) {
		client[i]->Close();
		delete client[i];
	}
}