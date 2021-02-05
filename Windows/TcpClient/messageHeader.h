#pragma once
#ifndef _MESSAGEHEADER_H_
#define _MESSAGEHEADER_H_
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};
//DataHead
struct DataHeader
{
	DataHeader()
	{
		dataLength = sizeof(DataHeader);
		cmd = 0;
	}
	short dataLength;
	short cmd;
};
//DataPackage
struct Login :public DataHeader
{
	Login() {
		dataLength = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	char userName[32] = {};
	char passWord[32] = {};
};

struct LoginResult :public DataHeader
{
	LoginResult() {
		dataLength = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
		result = 0;
	}
	int result;
};

struct Logout :public DataHeader
{
	Logout() {
		dataLength = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
	char userName[32] = {};

};

struct LogoutResult :public DataHeader
{
	LogoutResult() {
		dataLength = sizeof(LogoutResult);	
		cmd = CMD_LOGOUT_RESULT;
		result = 0;
	}
	int result;
};

struct NewUserJoin : public DataHeader
{
	NewUserJoin() {
		dataLength = sizeof(NewUserJoin);
		sock = 0;
		cmd = CMD_NEW_USER_JOIN;
	}
	int sock;
};

#endif