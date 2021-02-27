#include "selectServer.h"
#include "FDset.hpp"
#include "CELLLog.h"
#include "CELLSelect.h"

void SelectServer::onRun(CELLThread* pThread)
{
	FDset fdRead;
	int sock = getSock();
	while (pThread->Status()) {
		time4msg();
		//清理集合
		fdRead.zero();
		//将描述符加入集合
		fdRead.add(sock);
		//linux下的最大描述符
		SOCKET maxSock = sock;
		timeval t{ 0,0 };
		int ret = select(maxSock + 1, fdRead.getSet(), nullptr, nullptr, &t);
		if (ret < 0) {
			CELLLog_Info("Tcp.Server.On.Run.Select.Error.");
			pThread->Exit();
			break;
		}
		if (fdRead.has(sock)) {
			acConnection();
		}
	}
}

void SelectServer::Start(int nThread)
{
	TcpServer::Start<CELLSelect>(nThread);
}

