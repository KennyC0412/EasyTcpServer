#ifdef _WIN32
	#pragma once
#endif
#ifndef _CELLTIMESTAMP_H_
#define _CELLTIMESTAMP_H_

//#include <window.h>
#include <chrono>

class CELLTimestamp
{
public:
	CELLTimestamp()
	{
		update();
	}
	~CELLTimestamp()
	{}
	void update() {
		_begin = std::chrono::high_resolution_clock::now();
	}
	/*
	*	获取当前秒
	*/
	double getElapsedSecond() {
		return this->getElapsedTimeInMicroSec() * 0.000001;
	}
	/*
	*获取毫秒
	*/
	double getElapsedTimeInMilliSec() {
		return this->getElapsedTimeInMicroSec() * 0.001;

	}
	/*
	*获取微秒
	*/
	long long getElapsedTimeInMicroSec() {
		return std::chrono::duration_cast<std::chrono::microseconds>
			(std::chrono::high_resolution_clock::now() - _begin).count();
	}
protected:
	std::chrono::time_point<std::chrono::high_resolution_clock> _begin;
};

#endif	

