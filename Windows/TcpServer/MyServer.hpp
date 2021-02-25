#ifndef _MY_SERVER_HPP_
#define _MY_SERVER_HPP_

#include "server.h"
#include "CellServer.h"
#include "CELLLog.h"
#include "CELLMsgStream.hpp"

//用户自定义server
class MyServer :public TcpServer
{
public:
	//只被一个线程调用 线程安全
	virtual void onJoin(CELLClientPtr& client)
	{
		TcpServer::onJoin(client);
	}
	//可能会被多个线程调用 线程不安全
	virtual void onLeave(CELLClientPtr& client)
	{
		TcpServer::onLeave(client);
	}
	//可能会被多个线程调用 线程不安全
	virtual void onNetMsg(CELLServer* pserver, CELLClientPtr& pclient, DataHeader* dh)
	{
		TcpServer::onNetMsg(pserver, pclient, dh);
		switch (dh->cmd) {
		case CMD_LOGIN: {
			pclient->rstDtHeart();
			DataHeaderPtr ret = std::make_shared<S2C_Heart>();
			//pserver->sendTask(pclient, dynamic_cast<DataHeaderPtr&>(ret));*/
			if (SOCKET_ERROR == pclient->push(ret)) {
				CELLLog_Error("Buffer Full!");
			}
		}
		break;
		case CMD_LOGOUT:
		{
			CELLRecvStream r(dh);
			r.readInt16();
			r.getNetCMD();
			auto n1 = r.readInt8();
			auto n2 = r.readInt16();
			auto n3 = r.readInt32();
			auto n4 = r.readFloat();
			auto n5 = r.readDouble();
			uint32_t n = 0;
			char name[32] = {};
			auto n6 = r.readArray(name, 32);
			char pw[32]{};
			auto n7 = r.readArray(pw, 32);
			int aaa[10] = {};
			auto n8 = r.readArray(aaa, 10);
			DataHeaderPtr ret = std::make_shared<S2C_Heart>();
			if (SOCKET_ERROR == pclient->push(ret)) {
				CELLLog_Error("Buffer Full!");
			}
		}
		break;
		case CMD_HEART_C2S:
		{
			/*pclient->rstDtHeart();
			DataHeaderPtr ret = std::make_shared<S2C_Heart>();
			pserver->sendTask(pclient, dynamic_cast<DataHeaderPtr&>(ret));*/
		}
		break;
		default: {
			CELLLog_Warn("socket :", pclient->getSock(), " receive unknow message. ", dh->dataLength);
		}
		}
	}
	virtual void onRecv(CELLClientPtr&) { ++recvCount; }
};

#endif