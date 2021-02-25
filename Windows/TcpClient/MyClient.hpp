#include "client.h"
#include "CELLLog.h"
#include "CELLMsgStream.hpp"
#include "CELLConfig.hpp"

class MyClient :public TcpClient
{
public:
	MyClient(){
		_bCheckMsgID = CELLConfig::getInstance().hasKey("-sendMsg");
	}
	virtual void onNetMsg(DataHeader* dh)
	{
		switch (dh->cmd) {
		case CMD_LOGIN_RESULT:
		{
			LoginResult* loginResult = static_cast<LoginResult*>(dh);
			if (_bCheckMsgID) {
				if (loginResult->msgID != nRecvMsgID) {
					CELLLog_Error("Not equal msgID", loginResult->msgID, "RecvMsgID", nRecvMsgID);
				}
				++nRecvMsgID;
			}
			//std::cout << "receive:CMD_LOGIN_RESULT from server result:" << loginResult->result << "\tdata length:" << loginResult->dataLength << std::endl;
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			CELLRecvStream r(dh);
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
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin* newUser = static_cast<NewUserJoin*>(dh);
			//std::cout << "receive:CMD_NEW_USER_JOIN from server. New user's socket:" << newUser->sock << "\tdata length:" << newUser->dataLength << std::endl;
		}break;
		case CMD_HEART_S2C:
		{

		}
		break;
		case CMD_ERROR:
		{
			CELLLog::Error("receive:CMD_ERROR from server. ", "\tdata length:", dh->dataLength);
		}
		default:
			CELLLog::Error("socket :",_client->getSock(), " receive unknow message. ", dh->dataLength);
		}
	}
private:

};