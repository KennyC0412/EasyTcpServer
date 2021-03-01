#ifndef  _CELL_MSG_STREAM_H_
#define _CELL_MSG_STREAM_H_

#include "CELLStream.h"
#include "MsgHeader.h"

//接收消息数据字节流
class CELLRecvStream:public CELLStream
{
public:
	CELLRecvStream(DataHeader* dh):CELLStream((char *)dh,dh->dataLength){
		push(dh->dataLength);
	}

	uint16_t getNetCMD(){
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

	void setNetCMD(uint16_t cmd) {
		Write<uint16_t>(cmd);
	}

	void finish()
	{
		int pos = writeLength();
		setWritePos(0);
		Write<uint16_t>(pos);
		setWritePos(pos);
	}
private:

};
#endif // ! _CELL_MSG_STREAM_H_
