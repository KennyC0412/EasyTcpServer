#ifndef _CELL_STREAM_H_
#define _CELL_STREAM_H_
#include <cstdint>
#include <cstring>
#include "CELLLog.h"

//字节流基类
class CELLStream
{
public:
	CELLStream(char* data, int size, bool bDelete = true) :_nSize(size),_bDelete(bDelete) {
		_pBuff = data;
	}

	CELLStream(int size = 1024):_nSize(size), _bDelete(true){
		_pBuff = new char[_nSize];
	}

	virtual ~CELLStream(){
		if (_pBuff) {
			//delete[] _pBuff;
			_pBuff = nullptr;
		}
	}

	//判断是否可读
	inline bool canRead(int n) {
		return _nSize - _readPos >= n;
	}
	//判断是否可写
	inline bool canWrite(int n) {
		return _nSize - _writePos >= n;
	}
	//写入位置增加
	inline void push(int n) {
		_writePos += n;
	}
	//读出位置增加
	inline void pop(int n) {
		_readPos += n;
	}
	inline int readLength() {
		return _readPos;
	}
	inline int writeLength() {
		return _writePos;
	}
	inline void setWritePos(int pos) {
		_writePos = pos;
	}
	inline void setReadPos(int pos) {
		_readPos = pos;
	}

	//Read
	template <typename T>
	bool Read(T&n, bool noOffset = false) {
		//计算读取长度
		auto nLen = sizeof(T);
		if (canRead(nLen)) {
			//从缓冲区读入到T类型变量n中
			memcpy(&n, _pBuff + _readPos, nLen);
			if (noOffset == false)
				pop(nLen);
			return true;
		}
		CELLLog::Error("Read stream.");
		return false;
	}

	template<typename T>
	int readArray(T* data, uint32_t len)
	{
		//计算待读取数组长度
		uint32_t len1 = 0;
		Read<uint32_t>(len1, true);
		//判断数组是否能被放下
		if (len1 < len) {
			auto nLen = sizeof(T) * len1;
			if (canRead(nLen + sizeof(uint32_t))) {
				//跳过数组长度
				pop(sizeof(uint32_t));
				memcpy(data, _pBuff + _readPos, nLen);
				pop(nLen);
				return len1;
			}
		}
		CELLLog::Error("Read array.");
		return 0;
	}

	int8_t readInt8(int8_t = 0);
	int16_t readInt16(int16_t = 0);
	int32_t readInt32(int32_t = 0);
	float readFloat(float = 0.0f);
	double readDouble(double = 0.0);

	//Write
	template<typename T>
	inline bool Write(T n)
	{
		//计算写入大小
		int nLen = sizeof(T);
		//判断能否写入
		if (canWrite(nLen)) {
			memcpy(_pBuff, &n, nLen);
			push(nLen);
			return true;
		}
		CELLLog::Error("Write stream.");
		return false;
	}

	template<typename T>
	bool writeArray(T* data, uint32_t len)
	{
		//计算数组大小
		auto nLen = sizeof(T) * len;
		if (canWrite(nLen + sizeof(uint32_t))) {
			//写入数组长度
			writeInt32(len);
			//写入数据
			memcpy(_pBuff, data, nLen);
			push(nLen);
			return true;
		}
		CELLLog::Error("Write array");
		return false;
	}

	bool writeInt8(int8_t);
	bool writeInt16(int16_t);
	bool writeInt32(int32_t);
	bool writeFloat(float);
	bool writeDouble(double);


private:
	//缓冲区
	char* _pBuff = nullptr;
	//缓冲区大小
	int _nSize = 0;
	//已读数据尾部
	int _readPos = 0;
	//已写入数据尾部
	int _writePos = 0;
	//外部传入的数据块是否需要释放
	bool _bDelete;
};
#endif

