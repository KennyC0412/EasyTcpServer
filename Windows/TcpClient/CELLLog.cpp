#include "CELLLog.h"



void CELLLog::Info(const std::string& str)
{
	CELLLog* pLog = &getInstance();
	pLog->_taskServer.addTask([pLog, str]() {
		if (pLog->_logFile.is_open()) {
			auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			pLog->_logFile << std::put_time(std::localtime(&t), "[%Y-%m-%d %X]") << ' ';
			pLog->_logFile << str << std::endl;
			pLog->_logFile.seekp(0, std::ios::cur);
		}
		});
	std::cout << str << std::endl;
}

void CELLLog::Error(const std::string& str)
{
	CELLLog* pLog = &getInstance();
	pLog->_taskServer.addTask([pLog, str]() {
		if (pLog->_logFile.is_open()) {
			auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			pLog->_logFile << std::put_time(std::localtime(&t), "[%Y-%m-%d %X]") << ' ';
			pLog->_logFile << "Error:" << str << std::endl;
			pLog->_logFile.seekp(0, std::ios::cur);
		}
		});
	std::cout << "Error:" << str << std::endl;
}

void CELLLog::setLogPath(std::string logPath, char mode)
{
	if (_logFile.is_open())
		_logFile.close();
	switch (mode) {
	case 'w':_logFile.open(logPath, std::ios::out); break;
	case 'a':_logFile.open(logPath, std::ios::app); break;
	default: std::cout << "Wrong mode , using default mode.\n";
		_logFile.open(logPath, std::ios_base::trunc); break;
	}
	if (_logFile.is_open()) {
		Info("Success to set log path:<", logPath, "> in ", mode, " mode");
	}
	else {
		Error("failed to set log path:<", logPath, ">");
	}
}
