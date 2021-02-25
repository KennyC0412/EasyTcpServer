#include "CELLLog.h"



void CELLLog::Info(const std::string& str)
{
	Info("",str);
}

void CELLLog::Error(const std::string& str)
{
	Error("", str);
}

void CELLLog::Debug(const std::string& str)
{
	Debug("", str);
}

void CELLLog::Warn(const std::string& str)
{
	Warn("", str);

}

void CELLLog::setLogPath(const char* logName, char mode)
{
	if (_logFile.is_open())
		_logFile.close();
	auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::tm* now = std::localtime(&t);
	char logPath[256];
	sprintf(logPath, "%s[%d-%d-%d %d-%d-%d].txt", logName,now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
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

void CELLLog::makePre(const std::string& s,CELLLog* pLog)
{
	pLog->_taskServer.addTask([pLog,s]() {
		if (pLog->_logFile.is_open()) {
			auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			pLog->_logFile << std::put_time(std::localtime(&t), "[%Y-%m-%d %X]") << ' ';
			pLog->_logFile << s;
			pLog->_logFile.seekp(0, std::ios::cur);
		}
	});
	
}
