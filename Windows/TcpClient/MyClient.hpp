#include "client.h"
#include "CELLLog.h"

class MyClient :public TcpClient
{
public:
	virtual void onNetMsg(DataHeader* dh)
	{
		switch (dh->cmd) {
		case CMD_LOGIN_RESULT:
		{
			LoginResult* loginResult = static_cast<LoginResult*>(dh);
			//std::cout << "receive:CMD_LOGIN_RESULT from server result:" << loginResult->result << "\tdata length:" << loginResult->dataLength << std::endl;
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			LogoutResult* logoutResult = static_cast<LogoutResult*>(dh);
			//std::cout << "receive:CMD_LOGOUT_RESULT from server. result:" << logoutResult->result << "\tdata length:" << logoutResult->dataLength << std::endl;
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
			CELLLog::Error("socket :", _client->getSock(), " receive unknow message. ", dh->dataLength);
		}
	}

};