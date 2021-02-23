#ifndef  _CELL_MSG_STREAM_H_
#define _CELL_MSG_STREAM_H_

#include "CELLStream.h"
#include "messageHeader.h"

//接收消息数据字节流
class CELLRecvStream:public CELLStream
{
public:
	CELLRecvStream(DataHeader* dh):CELLStream((char *)dh,dh->dataLength){
		push(dh->dataLength);
	}

	uint16_t getNetCMD() {
		uint16_t cmd = CMD_ERROR;
		Read<uint16_t>(cmd);
		return cmd;
	}

private:

};

//写入消息数据字节流
class CELLSendStream :public CELLStream
{
public:
	CELLSendStream(char* data, int size, bool bDelete = false) :CELLStream(data, size, bDelete) {
		//预填充消息首部位置
		Write<uint16_t>(0);
	}

	CELLSendStream(int size = 1024) :CELLStream(size) {
		//预填充消息首部位置
		Write<uint16_t>(0);
	}

	void setNetCMD(uint16_t cmd){
		Write<uint16_t>(cmd);
	}

	//填入首部长度
	void finish()
	{
		int pos = writeLength();
		setWritePos(0);
		Write<uint16_t>(pos);
		setWritePos(pos);
	}

	char* data() {
		return _pBuff;
	}

	bool writeString(const char* str, int len) {
		return writeArray(str, len);
	}

	bool writeString(const char* str) {
		return writeArray(str, strlen(str));
	}
	bool writeString(std::string& str) {
		return writeArray(str.c_str(), str.length());
	}
};
#endif // ! _CELL_MSG_STREAM_H_
