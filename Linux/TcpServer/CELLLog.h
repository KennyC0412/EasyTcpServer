#ifndef _CELL_LOG_H_
#define _CELL_LOG_H_

#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include "CELLTask.h"

class CELLLog
{
#ifdef _DEBUG
	#define CELLLog_Debug(...) CELLLog::Debug(__VA_ARGS__)
#else
	#define CELLLog_Debug(...)
#endif // _DEBUG

#define CELLLog_Info(...)  CELLLog::Info(__VA_ARGS__)
#define CELLLog_Warn(...)  CELLLog::Warn(__VA_ARGS__)
#define CELLLog_Error(...)  CELLLog::Error(__VA_ARGS__)


public:
	~CELLLog() {
		_taskServer.Close();
		if (_logFile.is_open()) {
			_logFile.close();
			_logFile.clear();
		}
	};

	static CELLLog& getInstance()
	{
		static CELLLog sLog;
		return sLog;
	}

	static void Info(const std::string&);
	static void Error(const std::string&);
	static void Debug(const std::string&);
	static void Warn(const std::string&);

	template<typename ...Args>
	static void Info(const std::string& str,Args ...args){
		CELLLog* pLog = &getInstance();
		makePre("Info:",pLog);
		Echo(str,args...);
	}

	template<typename ...Args>
	static void Error(const std::string& str,Args...args){
		CELLLog* pLog = &getInstance();
		makePre("Error:",pLog);
		Echo(str,args...);
	}

	template<typename ...Args>
	static void Warn(const std::string& str, Args...args) {
		CELLLog* pLog = &getInstance();
		makePre("Warning:", pLog);
		Echo(str, args...);
	}

	template<typename ...Args>
	static void Debug(const std::string& str, Args...args) {
		CELLLog* pLog = &getInstance();
		makePre("Debug:", pLog);
		Echo(str, args...);
	}
	//设置日志路径
	void setLogPath(const char*, char);
	//写入记录前缀
	static void makePre(const std::string&,CELLLog*);

private:
	CELLLog() {
		_taskServer.Start();
	};
	CELLLog(const CELLLog&) = delete;
	CELLLog(CELLLog&) = delete;
	CELLLog& operator=(CELLLog) = delete;
private:
	template<typename T,typename ...Args>
	static void Echo(const T& str, Args ... args) {
		std::cout << str << ' ';
		CELLLog* pLog = &getInstance();
		pLog->_taskServer.addTask([pLog, str]() {
			if (pLog->_logFile.is_open()) {
				pLog->_logFile << str;
				pLog->_logFile.seekp(0, std::ios::cur);
			}
			});
		Echo(args...);
	}

	template<typename T>
	static void Echo(const T& str) {
		CELLLog* pLog = &getInstance();
		pLog->_taskServer.addTask([pLog, str]() {
			if (pLog->_logFile.is_open()) {
				pLog->_logFile << str << std::endl;
				pLog->_logFile.seekp(0, std::ios::cur);
			}
			});
		std::cout << str << std::endl;
	}
private:
	std::fstream _logFile;
	CELLTaskServer _taskServer;
};
#endif // !CELLLog
