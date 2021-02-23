#ifndef _CELL_STREAM_H_
#define _CELL_STREAM_H_
#include <cstdint>
#include <cstring>
#include "CELLLog.h"

//�ֽ�������
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

	//�ж��Ƿ�ɶ�
	inline bool canRead(int n) {
		return _nSize - _readPos >= n;
	}
	//�ж��Ƿ��д
	inline bool canWrite(int n) {
		return _nSize - _writePos >= n;
	}
	//д��λ������
	inline void push(int n) {
		_writePos += n;
	}
	//����λ������
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
		//�����ȡ����
		auto nLen = sizeof(T);
		if (canRead(nLen)) {
			//�ӻ��������뵽T���ͱ���n��
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
		//�������ȡ���鳤��
		uint32_t len1 = 0;
		Read<uint32_t>(len1, true);
		//�ж������Ƿ��ܱ�����
		if (len1 < len) {
			auto nLen = sizeof(T) * len1;
			if (canRead(nLen + sizeof(uint32_t))) {
				//�������鳤��
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
		//����д���С
		int nLen = sizeof(T);
		//�ж��ܷ�д��
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
		//���������С
		auto nLen = sizeof(T) * len;
		if (canWrite(nLen + sizeof(uint32_t))) {
			//д�����鳤��
			writeInt32(len);
			//д������
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
	//������
	char* _pBuff = nullptr;
	//��������С
	int _nSize = 0;
	//�Ѷ�����β��
	int _readPos = 0;
	//��д������β��
	int _writePos = 0;
	//�ⲿ��������ݿ��Ƿ���Ҫ�ͷ�
	bool _bDelete;
};
#endif

