#ifndef _CELL_CONFIG_HPP_
#define _CELL_CONFIG_HPP_

#include <string>
#include "CELLLog.h"
#include <map>
//用于读取配置数据
class CELLConfig {
public:
	
	static CELLConfig& getInstance() {
		static CELLConfig obj;
		return obj;
	}

	void Init(int argc, char* argv[]) {
		_exePath = argv[0];
		for (int i = 0; i < argc; i++) {
			madeCMD(argv[i]);
		}
	}

	void madeCMD(char* cmd) {
		char* val = strchr(cmd, '=');
		if (val) {
			*val = '\0';
			val++;
			_k_v[cmd] = val;
			CELLLog_Debug("madeCMD:", cmd, val);
		}
		else {
			_k_v[cmd] = "";
			CELLLog_Debug("madeCMD:", cmd);
		}
	}

	const char * getCMD(const char* key,const char *def) {
		auto it = _k_v.find(key);
		if (it == _k_v.end()) {
			CELLLog_Error("Key not found.",key);
		}
		else {
			def = it->second.c_str();
		}
		return def;
	}

	int getCMD(const char* key, int def) {
		auto it = _k_v.find(key);
		if (it == _k_v.end()) {
			CELLLog_Error("Key not found.", key);
		}
		else {
			def = atoi(it->second.c_str());
		}
		return def;
	}
private:
	CELLConfig() {}
	~CELLConfig(){}
	CELLConfig(CELLConfig&) = delete;
	CELLConfig& operator=(CELLConfig&) = delete;
private:
	//程序路径
	std::string _exePath;
	//配置传入的k-v数据
	std::map<std::string, std::string> _k_v;
};

#endif