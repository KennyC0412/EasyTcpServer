#define WIN32_LEAN_AND_MEAN
#include "pre.h"
#include "messageHeader.h"
#include "client.h"
#include "CELLtimestamp.hpp"
#include "MyClient.hpp"
#include "CELLLog.h"

bool g_bRun = true;
extern TcpClient* client[cCount];
std::atomic_int readyCount = 0;
std::atomic_int sendCount{ 0 };

void cmdThread()
{
	while (true) {
		char cmdBuf[256];
		std::cin >> cmdBuf;
		if (0 == strcmp(cmdBuf, "exit")) {
			std::cout << "cmd thread quit." << std::endl;
			g_bRun = false;
			break;
		}
		else {
			std::cout << "Wrong command.please input again:";
		}
	}
}

void recvThread(int begin,int end)
{
	CELLTimestamp t;
	t.update();
	while (g_bRun) {
		for (int i = begin; i < end; ++i) {
			if (t.getElapsedSecond() >= 3 && i%2 == 0)
				continue;
			client[i]->onRun();
		}
	}
}

void sendThread(int id)
{
	int c = cCount / tCount;
	int begin = (id - 1) * c;
	int end = id * c;
	for (int i = begin; i < end; ++i) {
		client[i] = new MyClient();
	}

	for (int i = begin; i < end; ++i) {
		client[i]->connServer("127.0.0.1", PORT);
	}
	CELLLog::Info("thread<", id, ">,Connect = <begin:", begin, ",end:", end, ">");

	readyCount++;
	while (readyCount < tCount) 
	{//等待所有线程完成连接
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
		for (int i = begin; i < end; ++i) {
			if (SOCKET_ERROR != client[i]->writeData(pdata)){
				++sendCount;
			}
			client[i]->onRun();
		}
	}

	for (int i = begin; i < end; ++i) {
		client[i]->Close();
		delete client[i];
	}
}