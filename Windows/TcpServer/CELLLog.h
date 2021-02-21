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

	template<typename ...Args>
	static void Info(const std::string& str,Args ...args){
		std::cout << str;
		CELLLog* pLog = &getInstance();
		pLog->_taskServer.addTask([pLog,str]() {
			if (pLog->_logFile.is_open()) {
				auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				pLog->_logFile << std::put_time(std::localtime(&t), "[%Y-%m-%d %X]") << ' ';
				pLog->_logFile << str;
				pLog->_logFile.seekp(0, std::ios::cur);
			}
			});
		print(args...);
	}

	template<typename ...Args>
	static void Error(const std::string& str,Args...args){
		std::cout << "Error:" << str;
		CELLLog* pLog = &getInstance();
		pLog->_taskServer.addTask([pLog, str]() {
			if (pLog->_logFile.is_open()) {
				auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
				pLog->_logFile << std::put_time(std::localtime(&t), "[%Y-%m-%d %X]") << ' ';
				pLog->_logFile << "Error:" << str;
				pLog->_logFile.seekp(0, std::ios::cur);
			}
			});
		print(args...);
	}
	//设置日志路径
	void setLogPath(std::string logPath, char mode);

private:
	CELLLog() {
		_taskServer.Start();
	};
	CELLLog(const CELLLog&) = delete;
	CELLLog(CELLLog&) = delete;
	CELLLog& operator=(CELLLog) = delete;
private:
	template<typename T,typename ...Args>
	static void print(const T& str, Args ... args) {
		std::cout << str << ' ';
		CELLLog* pLog = &getInstance();
		pLog->_taskServer.addTask([pLog, str]() {
			if (pLog->_logFile.is_open()) {
				pLog->_logFile << str;
				pLog->_logFile.seekp(0, std::ios::cur);
			}
			});
		print(args...);
	}

	template<typename T>
	static void print(const T& str) {
		CELLLog* pLog = &getInstance();
		pLog->_taskServer.addTask([pLog, str]() {
			if (pLog->_logFile.is_open()) {
				pLog->_logFile << str << std::endl;
				pLog->_logFile.seekp(0, std::ios::cur);
			}
			});
		std::cout << str << std::endl;
	}

	std::fstream _logFile;
	CELLTaskServer _taskServer;
};
#endif // !CELLLog
