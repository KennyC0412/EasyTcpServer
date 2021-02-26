#ifndef _FDSET_HPP_
#define _FDSET_HPP_

#include "pre.h"

class FDset
{
public:
	FDset() {
		int SocketNum = 10240;
#ifdef _WIN32
		 _nfdSize = sizeof(u_int) + (sizeof(SOCKET) * SocketNum);
#else
		 _nfdSize = SocketNum / (8 * sizeof(char));
#endif
		_pfdset = reinterpret_cast<fd_set *>(new char[_nfdSize]);
		memset(_pfdset, 0, _nfdSize);
	}

	~FDset()
	{
		if (_pfdset) {
			delete[] _pfdset;
			_pfdset = nullptr;
		}
	}

	inline void add(SOCKET s) {
		FD_SET(s, _pfdset);
	}

	inline void del(SOCKET s) {
		FD_CLR(s, _pfdset);
	}

	inline void zero() {
#ifdef _WIN32
		FD_ZERO(_pfdset);
#else
		memset(_pfdset, 0, nfdSize);
#endif
	}

	inline bool has(SOCKET s) {
		return  FD_ISSET(s,_pfdset);
	}

	inline fd_set* getSet() {
		return _pfdset;
	}

	void copy(FDset& set) {
		memcpy(_pfdset, set.getSet(), set._nfdSize);
	}
private:
	fd_set* _pfdset = nullptr;
	size_t  _nfdSize = 0;
};

#endif