#pragma once
#pragma comment(lib,"ws2_32.lib")
const short PORT = 8888;
const int BACKLOG = 4;


enum CMD
{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_ERROR
};
//DataHead
struct DataHeader
{
    short dataLength;
    short cmd;
};
//DataPackage
struct Login : public DataHeader
{
    Login() {
        dataLength = sizeof(Login);
        cmd = CMD_LOGIN;
    }
    char userName[32] = {};
    char passWord[32] = {};
};

struct LoginResult : public DataHeader
{
    LoginResult() {
        dataLength = sizeof(LoginResult);
        cmd = CMD_LOGIN_RESULT;
        result = 0;
    }
    int result;
};

struct Logout : public DataHeader
{
    Logout() {
        dataLength = sizeof(Logout);
        cmd = CMD_LOGOUT;
    }
    char userName[32] = {};
};

struct LogoutResult : public DataHeader
{
    LogoutResult() {
        dataLength = sizeof(LogoutResult);
        cmd = CMD_LOGOUT_RESULT;
        result = 0;

    }
    int result;
};

const char welcome[] = "Hello, This is server.\n";

