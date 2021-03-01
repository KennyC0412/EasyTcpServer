#include "CELLSelect.h"
#include "CELLLog.h"

bool CELLSelect::core()
{
		//linux下的最大描述符
		if (client_change) {
			_fdRead.zero();
			client_change = false;
			for (auto& iter : _clients) {
				_fdRead.add(iter.first);
				if (_maxSock < iter.first) {
					_maxSock = iter.first;
				}
			}
			_fdRead_back.copy(_fdRead);
		}
		else {
			_fdRead.copy(_fdRead_back);
		}
		int ret;
		bool NeedWrite = false;
		_fdWrite.zero();
		//检测需要写的客户端
		for (auto& iter : _clients) {
			if (iter.second->needWrite()) {
				NeedWrite = true;
				_fdWrite.add(iter.first);
			}
		}
		timeval t{ 0,0 };
		if (NeedWrite) {
			ret = select(_maxSock + 1, _fdRead.getSet(), _fdWrite.getSet(), nullptr, &t);
		}
		else {
			ret = select(_maxSock + 1, _fdRead.getSet(), nullptr, nullptr, &t);
		}
		if (ret < 0) {
			CELLLog_Error("CellServer.OnRun.Select.Error");
			return false;
		}
		else if (ret == 0) {
			return true;
		}
		readData();
		writeData();
		return true;
}



void CELLSelect::writeData()
{
#ifdef _WIN32
	fd_set* temp = _fdWrite.getSet();
	for (size_t i = 0; i < temp->fd_count; ++i) {
		auto iter = _clients.find(temp->fd_array[i]);
		if (iter != _clients.end()) {
			if (-1 == iter->second->sendData()) {
				ClientLeave(iter->second);
				_clients.erase(iter);
			}
		}
	}
#else
	for (auto iter = _clients.begin(); iter != _clients.end();) {
		if (iter->second->needWrite() && _fdWrite.has(iter->first)) {
			if (-1 == iter->second->sendData()) {
				ClientLeave(iter->second);
				iter = _clients.erase(iter);
			}
			if (iter != _clients.end())
				iter++;
		}
		else {
			iter++;
		}
	}
#endif
}

void CELLSelect::readData()
{
#ifdef _WIN32
	fd_set* temp = _fdRead.getSet();
	for (size_t i = 0; i < temp->fd_count; i++) {
		auto iter = _clients.find(temp->fd_array[i]);
		if (iter != _clients.end()) {
			if (-1 == recvData(iter->second)) {
				ClientLeave(iter->second);
				_clients.erase(iter);
			}
		}
		else {
			CELLLog_Error("End of iterator");
		}
	}
#else
	for (auto iter = _clients.begin(); iter != _clients.end();) {
		if (_fdRead.has(iter->first)) {
			if (-1 == recvData(iter->second)) {
				ClientLeave(iter->second);
				iter = _clients.erase(iter);
			}
			if (iter != _clients.end())
				iter++;
		}
		else {
			iter++;
		}
	}
#endif
}

