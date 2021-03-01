#ifndef _MY_SERVER_HPP_
#define _MY_SERVER_HPP_

#include "server.h"
#include "CellServer.h"
#include "CELLLog.h"
#include "CELLMsgStream.hpp"
#include "CELLConfig.hpp"
#include "epollServer.h"

//�û��Զ���server
class MyServer:public EpollServer
{
public:
	MyServer(int maxClient):EpollServer(maxClient)
	{
		_bSendBack = CELLConfig::getInstance().hasKey("-sendback");
		_bSendFull = CELLConfig::getInstance().hasKey("-sendFull");
		_bCheckMsgID = CELLConfig::getInstance().hasKey("-sendMsg");
	}
	//ֻ��һ���̵߳��� �̰߳�ȫ
	virtual void onJoin()
	{
		TcpServer::onJoin();
	}
	//���ܻᱻ����̵߳��� �̲߳���ȫ
	virtual void onLeave()
	{
		TcpServer::onLeave();
	}
	//���ܻᱻ����̵߳��� �̲߳���ȫ
	virtual void onNetMsg(CELLServer* pserver, CELLClientPtr& pclient, DataHeader* dh)
	{
		TcpServer::onNetMsg(pserver, pclient, dh);
		switch (dh->cmd) {
		case CMD_LOGIN: {
			Login* login = static_cast<Login*>(dh);
			pclient->rstDtHeart();
			if (_bCheckMsgID) {
				if (login->msgID != pclient->nRecvMsgID) {
					CELLLog_Error("Not equal msgID", login->msgID, "RecvMsgID",pclient->nRecvMsgID);
				}
				++pclient->nRecvMsgID;
			}
			//pserver->sendTask(pclient, dynamic_cast<DataHeaderPtr&>(ret));*/
			if (_bSendBack) {
				DataHeaderPtr ret = std::make_shared<LoginResult>();
				ret->msgID = pclient->nSendMsgID++;
				if (SOCKET_ERROR == pclient->push(ret)) {
					if (_bSendFull) {
						CELLLog_Warn("Buffer Full!");
					}
				}
			}
		}
		break;
		case CMD_LOGOUT:
		{
			//CELLRecvStream r(dh);
			//r.readInt16();
			//r.getNetCMD();
			//auto n1 = r.readInt8();
			//auto n2 = r.readInt16();
			//auto n3 = r.readInt32();
			//auto n4 = r.readFloat();
			//auto n5 = r.readDouble();
			//uint32_t n = 0;
			//char name[32] = {};
			//auto n6 = r.readArray(name, 32);
			//char pw[32]{};
			//auto n7 = r.readArray(pw, 32);
			//int aaa[10] = {};
			//auto n8 = r.readArray(aaa, 10);
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
	virtual void onRecv() { TcpServer::onRecv(); }

public:
	bool _bSendBack;
	bool _bSendFull;
	bool _bCheckMsgID;
};

#endif